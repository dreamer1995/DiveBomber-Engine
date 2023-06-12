#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::DX
{
	class GPUAdapter final
	{
	public:
		GPUAdapter();
		wrl::ComPtr<IDXGIAdapter4> GetAdapter() noexcept;
	private:
		wrl::ComPtr<IDXGIAdapter4> dxgiAdapter;
	};
}