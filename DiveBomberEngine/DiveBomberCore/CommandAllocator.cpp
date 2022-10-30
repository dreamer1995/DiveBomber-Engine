#include "CommandAllocator.h"

CommandAllocator::CommandAllocator(ID3D12Device2* device, D3D12_COMMAND_LIST_TYPE type)
{
    HRESULT hr;
    GFX_THROW_INFO(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));
}

ID3D12CommandAllocator* CommandAllocator::GetCommandAllocator() noexcept
{
    return commandAllocator.Get();
}