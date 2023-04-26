#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::DX
{
	class GPUAdapter final
	{
	public:
		GPUAdapter();
		IDXGIAdapter4* GetAdapter() noexcept;
	private:
		wrl::ComPtr<IDXGIAdapter4> dxgiAdapter;
	};
}