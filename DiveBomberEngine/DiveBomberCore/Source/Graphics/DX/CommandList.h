#pragma once
#include "..\GraphicsHeader.h"

#include <memory>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::DX
{
	class UploadBuffer;
	class ResourceStateTracker;

	class CommandList final
	{
	public:
		CommandList(wrl::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE inputType);
		~CommandList();
		void Reset();
		[[nodiscard]] wrl::ComPtr<ID3D12GraphicsCommandList2> GetGraphicsCommandList() const;
		void Close();
		[[nodiscard]] D3D12_COMMAND_LIST_TYPE GetCommandListType() const noexcept;
	private:
		D3D12_COMMAND_LIST_TYPE type;
		wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;
		std::unique_ptr<UploadBuffer> uploadBuffer;
		std::unique_ptr<ResourceStateTracker> resourceStateTracker;
	};
}

