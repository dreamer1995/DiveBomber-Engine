#include "CommandManager.h"

namespace DiveBomber::DX
{
    using namespace DEException;

    CommandManager::CommandManager(ID3D12Device2* inputDevice, std::shared_ptr<FenceManager> inputFenceManager, D3D12_COMMAND_LIST_TYPE intputType)
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

        fenceManager = inputFenceManager;

        fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
        assert(fenceEvent && "Failed to create fence event.");
    }

    CommandManager::~CommandManager()
    {
        CloseHandle(fenceEvent);
    }

    ID3D12CommandQueue* CommandManager::GetCommandQueue() noexcept
    {
        return commandQueue.Get();
    }

    void CommandManager::Signal() noexcept
    {
        fenceValue++;
        commandQueue->Signal(fenceManager->GetFence(), fenceValue);
    }

    void CommandManager::Flush() noexcept
    {
        Signal();
        fenceManager->WaitForFenceValue(fenceValue, fenceEvent);
    }

    wrl::ComPtr<ID3D12CommandAllocator> CommandManager::CreateCommandAllocator()
    {
        wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
        HRESULT hr;
        GFX_THROW_INFO(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));
        return commandAllocator;
    }

    wrl::ComPtr<ID3D12GraphicsCommandList2> CommandManager::CreateCommandList(ID3D12CommandAllocator* commandAllocator)
    {
        wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;
        HRESULT hr;
        GFX_THROW_INFO(device->CreateCommandList(0, type, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));
        commandList->Close();
        return commandList;
    }

    wrl::ComPtr <ID3D12GraphicsCommandList2> CommandManager::GetCommandList()
    {
        wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
        wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;

        HRESULT hr;
        if (!commandAllocatorQueue.empty() && fenceManager->IsFenceComplete(commandAllocatorQueue.front().fenceValue))
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

        return commandList;
    }

    uint64_t CommandManager::ExecuteCommandList(ID3D12GraphicsCommandList2* commandList)
    {
        commandList->Close();

        ID3D12CommandAllocator* commandAllocator;
        UINT dataSize = sizeof(commandAllocator);

        HRESULT hr;
        GFX_THROW_INFO(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

        ID3D12CommandList* const commandLists[] = { commandList };

        commandQueue->ExecuteCommandLists(1, commandLists);
        Signal();

        commandAllocatorQueue.emplace(CommandAllocatorInfo{ fenceValue, commandAllocator });
        commandListQueue.push(commandList);

        // The ownership of the command allocator has been transferred to the ComPtr
        // in the command allocator queue. It is safe to release the reference 
        // in this temporary COM pointer here.
        commandAllocator->Release();

        return fenceValue;
    }

    void CommandManager::WaitForFenceValue(uint64_t inputFenceValue) noexcept
    {
        fenceManager->WaitForFenceValue(inputFenceValue, fenceEvent);
    }
}