#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::DX
{
	class DXDevice final
	{
	public:
		DXDevice(wrl::ComPtr<IDXGIAdapter4> adapter);
		wrl::ComPtr<ID3D12Device2> GetDecive() noexcept;
	private:
		wrl::ComPtr<ID3D12Device2> dxDevice;
	};
}