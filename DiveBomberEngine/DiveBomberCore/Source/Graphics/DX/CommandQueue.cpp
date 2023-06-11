#include "CommandQueue.h"

namespace DiveBomber::DX
{
    using namespace DEException;

    CommandQueue::CommandQueue(ID3D12Device2* inputDevice, D3D12_COMMAND_LIST_TYPE intputType)
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

        fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
        assert(fenceEvent && "Failed to create fence event.");
    }

    CommandQueue::~CommandQueue()
    {
        CloseHandle(fenceEvent);
    }

    ID3D12CommandQueue* CommandQueue::GetCommandQueue() noexcept
    {
        return commandQueue.Get();
    }

    uint64_t CommandQueue::Signal() noexcept
    {
        fenceValue++;
        commandQueue->Signal(fence.Get(), fenceValue);
        return fenceValue;
    }

    void CommandQueue::Flush() noexcept
    {
        WaitForFenceValue(Signal());
    }

    wrl::ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
    {
        wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
        HRESULT hr;
        GFX_THROW_INFO(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));
        return commandAllocator;
    }

    wrl::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::CreateCommandList(wrl::ComPtr<ID3D12CommandAllocator> commandAllocator)
    {
        wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;
        HRESULT hr;
        GFX_THROW_INFO(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
        return commandList;
    }

    wrl::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::GetCommandList()
    {
        wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
        wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;

        HRESULT hr;
        if (!commandAllocatorQueue.empty() && IsFenceComplete(commandAllocatorQueue.front().fenceValue))
        {
            commandAllocator = commandAllocatorQueue.front().commandAllocator;
            commandAllocatorQueue.pop();

            GFX_THROW_INFO(commandAllocator->Reset());
        }
        else
        {
            commandAllocator = CreateCommandAllocator();
        }

        if (!commandListQueue.empty())
        {
            commandList = commandListQueue.front();
            commandListQueue.pop();

            GFX_THROW_INFO(commandList->Reset(commandAllocator.Get(), nullptr));
        }
        else
        {
            commandList = CreateCommandList(commandAllocator.Get());
        }

        // Associate the command allocator with the command list so that it can be
        // retrieved when the command list is executed.
        GFX_THROW_INFO(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get()));

        return commandList.Get();
    }

    uint64_t CommandQueue::ExecuteCommandList(wrl::ComPtr<ID3D12GraphicsCommandList2> commandList)
    {
        commandList->Close();

        ID3D12CommandAllocator* commandAllocator;
        UINT dataSize = sizeof(commandAllocator);

        HRESULT hr;
        GFX_THROW_INFO(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

        ID3D12CommandList* const commandLists[] = { commandList.Get()};

        commandQueue->ExecuteCommandLists(1, commandLists);

        commandAllocatorQueue.emplace(CommandAllocatorInfo{ Signal(), commandAllocator});
        commandListQueue.push(std::move(commandList));

        // The ownership of the command allocator has been transferred to the ComPtr
        // in the command allocator queue. It is safe to release the reference 
        // in this temporary COM pointer here.
        commandAllocator->Release();

        return fenceValue;
    }

    void CommandQueue::WaitForFenceValue(uint64_t inputFenceValue) noexcept
    {
        if (!IsFenceComplete(inputFenceValue))
        {
            fence->SetEventOnCompletion(inputFenceValue, fenceEvent);
            ::WaitForSingleObject(fenceEvent, DWORD_MAX);
        }
    }

    bool CommandQueue::IsFenceComplete(uint64_t inputFenceValue) noexcept
    {
        return fence->GetCompletedValue() >= inputFenceValue;
    }
}