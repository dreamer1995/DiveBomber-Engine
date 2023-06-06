#include "CommandContext.h"
namespace DiveBomber::DX
{
	using namespace DEGraphics;
	CommandContext::CommandContext(Graphics& inputGfx, D3D12_COMMAND_LIST_TYPE intputType)
		:
		gfx(inputGfx)
	{
		type = intputType;
		commandQueue = gfx.GetCommandQueue(type);
		commandList = commandQueue->GetCommandList();
	}
	void CommandContext::BindBuffer(ID3D12Resource* destBuf, ID3D12Resource* intermediateBuf, UINT numSubresources, D3D12_SUBRESOURCE_DATA subData[])
	{
		UpdateSubresources(commandList,
			destBuf, intermediateBuf,
			0, 0, numSubresources, subData);
	}
	void CommandContext::ExecuteCommandList()
	{
		uint64_t fenceValue = commandQueue->ExecuteCommandList(commandList);
		commandQueue->WaitForFenceValue(fenceValue);
	}
}