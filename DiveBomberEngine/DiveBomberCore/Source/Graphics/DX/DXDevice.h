#pragma once
#include "..\GraphicsHeader.h"

#include <dxgi1_6.h>

namespace DiveBomber::DX
{
	class DXDevice final
	{
	public:
		DXDevice(const wrl::ComPtr<IDXGIAdapter4> adapter);
		[[nodiscard]] wrl::ComPtr<ID3D12Device2> GetDecive() const noexcept;
	private:
		wrl::ComPtr<ID3D12Device2> dxDevice;
	};
}