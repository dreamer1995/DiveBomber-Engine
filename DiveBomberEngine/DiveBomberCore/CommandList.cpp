#include "CommandList.h"

CommandList::CommandList(ID3D12Device2* device, ID3D12CommandAllocator* commandAllocator, D3D12_COMMAND_LIST_TYPE type)
{
    HRESULT hr;
    GFX_THROW_INFO(device->CreateCommandList(0, type, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));
    commandList->Close();
}

ID3D12GraphicsCommandList* CommandList::GetCommandList() noexcept
{
    return commandList.Get();
}