#pragma once
#include "GraphicsResource.h"
#include "WindowResource.h"

class Fence
{
public:
	Fence(ID3D12Device2* device);
	ID3D12Fence* GetFence() noexcept;
private:
	wrl::ComPtr<ID3D12Fence> fence;
};
