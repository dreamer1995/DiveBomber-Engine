#pragma once
#include "..\GraphicsHeader.h"

namespace DiveBomber::DX
{
	class DXDevice final
	{
	public:
		DXDevice(const wrl::ComPtr<IDXGIAdapter4> adapter);
		[[nodiscard]] wrl::ComPtr<ID3D12Device10> GetDevice() const noexcept;
	private:
		wrl::ComPtr<ID3D12Device10> dxDevice;
	};
}