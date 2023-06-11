#pragma once
#include "..\GraphicsResource.h"
#include "..\Graphics.h"

namespace DiveBomber::DX
{
	class CommandContext final
	{
	public:
		CommandContext(DEGraphics::Graphics& inputGfx, D3D12_COMMAND_LIST_TYPE intputType);
		void BindBuffer(ID3D12Resource* destBuf, ID3D12Resource* intermediateBuf, UINT numSubresources, D3D12_SUBRESOURCE_DATA subData[]);
		void ExecuteCommandList();
	private:
		DEGraphics::Graphics& gfx;
		D3D12_COMMAND_LIST_TYPE type;
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;
		DX::CommandQueue* commandQueue;
	};
}

