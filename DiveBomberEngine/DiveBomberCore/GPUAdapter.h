#pragma once
#include "GraphicsResource.h"
#include "WindowResource.h"

class GPUAdapter
{
public:
	GPUAdapter();
	IDXGIAdapter4* GetAdapter() noexcept;
private:
	wrl::ComPtr<IDXGIAdapter4> dxgiAdapter;
};

