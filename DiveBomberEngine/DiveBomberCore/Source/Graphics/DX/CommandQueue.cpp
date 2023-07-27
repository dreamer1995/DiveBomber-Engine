#include "CommandQueue.h"

#include "..\..\Exception\GraphicsException.h"
#include "CommandLIst.h"

namespace DiveBomber::DX
{
    using namespace DEException;

    CommandQueue::CommandQueue(wrl::ComPtr<ID3D12Device2> inputDevice, D3D12_COMMAND_LIST_TYPE intputType)
    {
        type = intputType;
        device = inputDevice;
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = type;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        HRESULT hr;
        GFX_THROW_INFO(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)));
        GFX_THROW_INFO(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

        switch (type)
        {
        case D3D12_COMMAND_LIST_TYPE_COPY:
            commandQueue->SetName(L"Copy Command Queue");
            break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
            commandQueue->SetName(L"Compute Command Queue");
            break;
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
            commandQueue->SetName(L"Direct Command Queue");
            break;
        }

        commandListsGarbageCollectionThread = std::thread(&CommandQueue::CommandListsGarbageCollection, this);
    }

    CommandQueue::~CommandQueue()
    {
        processCommandListsGarbageCollection = false;
        commandListsGarbageCollectionThread.join();
    }

    wrl::ComPtr<ID3D12CommandQueue> CommandQueue::GetCommandQueue() const noexcept
    {
        return commandQueue;
    }

    uint64_t CommandQueue::Signal() noexcept
    {
        fenceValue++;
        commandQueue->Signal(fence.Get(), fenceValue);
        return fenceValue;
    }

    void CommandQueue::Flush() noexcept
    {
        std::unique_lock<std::mutex> lock(commandListsGarbageCollectionThreadMutex);
        commandListsGarbageCollectionThreadConditionVariable.wait(lock, [this] {return inFlightCommandLists.Empty(); });

        // In case the command queue was signaled directly 
        // using the CommandQueue::Signal method then the 
        // fence value of the command queue might be higher than the fence
        // value of any of the executed command lists.
        WaitForFenceValue(Signal());
    }

    std::shared_ptr<CommandList> CommandQueue::GetCommandList()
    {
        std::shared_ptr<CommandList> commandList;

        // If there is a command list on the queue.
        if (!availableCommandLists.Empty())
        {
            availableCommandLists.TryPop(commandList);
        }
        else
        {
            // Otherwise create a new command list.
            commandList = std::make_shared<CommandList>(device, type);
        }

        return commandList;
    }

    // Execute a command list.
    // Returns the fence value to wait for for this command list.
    uint64_t CommandQueue::ExecuteCommandList(std::shared_ptr<CommandList> commandList)
    {
        return ExecuteCommandLists(std::vector<std::shared_ptr<CommandList>>({ commandList }));
    }

    uint64_t CommandQueue::ExecuteCommandLists(const std::vector<std::shared_ptr<CommandList>>& commandLists)
    {
        std::vector<ID3D12CommandList*> d3d12CommandLists;
        d3d12CommandLists.reserve(commandLists.size());

        std::vector<std::shared_ptr<CommandList>> inFlightQueue;
        inFlightQueue.reserve(commandLists.size());

        for (auto commandList : commandLists)
        {
            commandList->Close();
            d3d12CommandLists.emplace_back(commandList->GetGraphicsCommandList().Get());

            inFlightQueue.emplace_back(commandList);
        }

        UINT numCommandLists = static_cast<UINT>(d3d12CommandLists.size());

        commandQueue->ExecuteCommandLists(numCommandLists, d3d12CommandLists.data());
        Signal();

        for (auto commandList : inFlightQueue)
        {
            inFlightCommandLists.Push({ fenceValue, commandList });
        }

        return fenceValue;
    }

    void CommandQueue::WaitForFenceValue(const uint64_t inputFenceValue) noexcept
    {
        if (!IsFenceComplete(inputFenceValue))
        {
            HANDLE fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
            assert(fenceEvent && "Failed to create fence event.");

            fence->SetEventOnCompletion(inputFenceValue, fenceEvent);
            ::WaitForSingleObject(fenceEvent, DWORD_MAX);

            ::CloseHandle(fenceEvent);
        }
    }

    bool CommandQueue::IsFenceComplete(uint64_t inputFenceValue) const noexcept
    {
        return fence->GetCompletedValue() >= inputFenceValue;
    }

    void CommandQueue::CommandListsGarbageCollection()
    {
        std::unique_lock<std::mutex> lock(commandListsGarbageCollectionThreadMutex, std::defer_lock);

        while (processCommandListsGarbageCollection)
        {
            CommandListInfo commandListInfo;

            lock.lock();
            while (inFlightCommandLists.TryPop(commandListInfo))
            {
                uint64_t commandListFenceValue = std::get<0>(commandListInfo);
                std::shared_ptr<CommandList> commandList = std::get<1>(commandListInfo);

                WaitForFenceValue(commandListFenceValue);

                commandList->Reset();

                availableCommandLists.Push(commandList);
            }
            lock.unlock();
            commandListsGarbageCollectionThreadConditionVariable.notify_one();

            std::this_thread::yield();
        }
    }
}