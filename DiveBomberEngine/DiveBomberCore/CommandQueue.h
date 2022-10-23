#pragma once
#include "GraphicsResource.h"

class CommandQueue
{
public:
	CommandQueue(ID3D12Device2* device);
	ID3D12CommandQueue* GetCommandQueue() noexcept;
private:
	wrl::ComPtr<ID3D12CommandQueue> commandQueue;
};
