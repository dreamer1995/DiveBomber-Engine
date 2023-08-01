#include "CommandList.h"

#include "..\..\Exception\GraphicsException.h"
#include "..\Graphics.h"
#include "..\Component\UploadBuffer.h"

namespace DiveBomber::DX
{
	//using namespace DEGraphics;
	using namespace DEException;
	using namespace Component;
	CommandList::CommandList(wrl::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE inputType)
		:
		type(inputType)
	{
		HRESULT hr;
		GFX_THROW_INFO(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));
		GFX_THROW_INFO(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
		
		uploadBuffer = std::make_shared<UploadBuffer>(device);
	}
	
	void CommandList::Reset()
	{
		HRESULT hr;
		GFX_THROW_INFO(commandList->Reset(commandAllocator.Get(), nullptr));
	}

	wrl::ComPtr<ID3D12GraphicsCommandList2> CommandList::GetGraphicsCommandList() const
	{
		return commandList;
	}

	void CommandList::Close()
	{
		HRESULT hr;
		GFX_THROW_INFO(commandList->Close());
	}

	D3D12_COMMAND_LIST_TYPE CommandList::GetCommandListType() const noexcept
	{
		return type;
	}
}