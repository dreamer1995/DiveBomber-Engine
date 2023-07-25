#pragma once
#include "..\GraphicsHeader.h"

#include <dxgi1_6.h>

namespace DiveBomber::DX
{
	class GPUAdapter final
	{
	public:
		GPUAdapter();
		[[nodiscard]] wrl::ComPtr<IDXGIAdapter4> GetAdapter() const noexcept;
	private:
		wrl::ComPtr<IDXGIAdapter4> dxgiAdapter;
	};
}