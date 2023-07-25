#pragma once
#include "..\GraphicsHeader.h"

#include <memory>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::DX
{
	class CommandQueue;

	class CommandContext final
	{
	public:
		CommandContext(DEGraphics::Graphics& inputGfx, D3D12_COMMAND_LIST_TYPE intputType);
		void BindBuffer(wrl::ComPtr<ID3D12Resource> destBuf, wrl::ComPtr<ID3D12Resource> intermediateBuf, UINT numSubresources, D3D12_SUBRESOURCE_DATA subData[]) const;
		void ExecuteCommandList() const;
	private:
		DEGraphics::Graphics& gfx;
		D3D12_COMMAND_LIST_TYPE type;
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;
		std::shared_ptr<CommandQueue> commandQueue;
	};
}

