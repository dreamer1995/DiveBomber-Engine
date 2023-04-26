#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::DX
{
	using namespace DEException;

	class CommandQueue final
	{
	public:
		CommandQueue(ID3D12Device2* device);
		ID3D12CommandQueue* GetCommandQueue() noexcept;
	private:
		wrl::ComPtr<ID3D12CommandQueue> commandQueue;
	};
}
