#pragma once
#include "..\GraphicsResource.h"

namespace DiveBomber::DX
{
	class Viewport final
	{
	public:
		Viewport();
		[[nodiscard]] D3D12_VIEWPORT GetViewport() const noexcept;
		void ResizeViewport(uint32_t width, uint32_t height) noxnd;
		void Bind(wrl::ComPtr<ID3D12GraphicsCommandList2> commandList) noxnd;
	private:
		D3D12_VIEWPORT viewport;
	};
}