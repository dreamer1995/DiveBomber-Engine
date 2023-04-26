#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::DX
{
	class DXDevice final
	{
	public:
		DXDevice(IDXGIAdapter4* adapter);
		ID3D12Device2* GetDecive()noexcept;
	private:
		wrl::ComPtr<ID3D12Device2> dxDevice;
	};
}