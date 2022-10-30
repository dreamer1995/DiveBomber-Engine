#include "Fence.h"

Fence::Fence(ID3D12Device2* device)
{
    HRESULT hr;
    GFX_THROW_INFO(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
}

ID3D12Fence* Fence::GetFence() noexcept
{
    return fence.Get();
}