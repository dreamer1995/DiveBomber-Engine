#pragma once
#include "GraphicsResource.h"
#include "WindowResource.h"

class CommandList
{
public:
	CommandList(ID3D12Device2* device, ID3D12CommandAllocator* commandAllocator, D3D12_COMMAND_LIST_TYPE type);
	ID3D12GraphicsCommandList* GetCommandList() noexcept;
private:
	wrl::ComPtr<ID3D12GraphicsCommandList> commandList;
};

