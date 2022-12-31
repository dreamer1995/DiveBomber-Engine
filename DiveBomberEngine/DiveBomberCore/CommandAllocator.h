#pragma once
#include "GraphicsResource.h"
#include "WindowResource.h"

class CommandAllocator final
{
public:
	CommandAllocator(ID3D12Device2* device, D3D12_COMMAND_LIST_TYPE type);
	ID3D12CommandAllocator* GetCommandAllocator() noexcept;
private:
	wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
};
