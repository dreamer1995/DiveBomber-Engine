#pragma once
#include "GraphicsResource.h"
#include "WindowResource.h"

class GPUAdapter
{
public:
	GPUAdapter(std::shared_ptr<DxgiInfoManager> inputInfoManager);
	IDXGIAdapter4* GetAdapter() noexcept;
private:
	wrl::ComPtr<IDXGIAdapter4> dxgiAdapter;
	std::shared_ptr<DxgiInfoManager> infoManager;
};

