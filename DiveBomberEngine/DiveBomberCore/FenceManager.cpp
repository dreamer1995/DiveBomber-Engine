#include "FenceManager.h"

FenceManager::FenceManager(ID3D12Device2* device, uint64_t fenceValue)
{
    HRESULT hr;
    GFX_THROW_INFO(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
}

ID3D12Fence* FenceManager::GetFence() noexcept
{
    return fence.Get();
}

bool FenceManager::IsFenceComplete(uint64_t fenceValue) noexcept
{
    return fence->GetCompletedValue() >= fenceValue;
}

void FenceManager::WaitForFenceValue(uint64_t fenceValue, HANDLE fenceEvent) noexcept
{
    if (!IsFenceComplete(fenceValue))
    {
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        ::WaitForSingleObject(fenceEvent, DWORD_MAX);
    }
}