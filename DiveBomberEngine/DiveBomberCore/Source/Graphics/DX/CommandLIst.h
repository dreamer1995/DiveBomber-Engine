#pragma once
#include "..\GraphicsHeader.h"

#include <memory>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::DX
{
	class CommandList final
	{
	public:
		CommandList(wrl::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE inputType);
		void Reset();
		[[nodiscard]] wrl::ComPtr<ID3D12GraphicsCommandList2> GetGraphicsCommandList() const;
		void Close();
		[[nodiscard]] D3D12_COMMAND_LIST_TYPE GetCommandListType() const noexcept;
	private:
		D3D12_COMMAND_LIST_TYPE type;
		wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;
	};
}

