#pragma once
#include "GraphicsResource.h"

class DXDevice
{
public:
	DXDevice(IDXGIAdapter4* adapter, std::shared_ptr<DxgiInfoManager> inputInfoManager);
	ID3D12Device2* GetDecive()noexcept;
private:
	wrl::ComPtr<ID3D12Device2> dxDevice;
	std::shared_ptr<DxgiInfoManager> infoManager;
};

