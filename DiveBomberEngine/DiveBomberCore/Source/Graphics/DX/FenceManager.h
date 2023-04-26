#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::DX
{
	class FenceManager final
	{
	public:
		FenceManager(ID3D12Device2* device, uint64_t fenceValue = 0);
		ID3D12Fence* GetFence() noexcept;
		bool IsFenceComplete(uint64_t fenceValue) noexcept;
		//Should fence event here?
		void WaitForFenceValue(uint64_t fenceValue, HANDLE fenceEvent) noexcept;
	private:
		wrl::ComPtr<ID3D12Fence> fence;
	};
}
