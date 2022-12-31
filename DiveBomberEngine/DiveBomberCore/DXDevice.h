#pragma once
#include "GraphicsResource.h"

class DXDevice final
{
public:
	DXDevice(IDXGIAdapter4* adapter);
	ID3D12Device2* GetDecive()noexcept;
private:
	wrl::ComPtr<ID3D12Device2> dxDevice;
};

