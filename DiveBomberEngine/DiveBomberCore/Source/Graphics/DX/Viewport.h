#pragma once
#include "..\GraphicsHeader.h"

namespace DiveBomber::DX
{
	class Viewport final
	{
	public:
		Viewport();
		[[nodiscard]] D3D12_VIEWPORT GetViewport() const noexcept;
		void ResizeViewport(uint32_t width, uint32_t height) noxnd;
		void Bind(wrl::ComPtr<ID3D12GraphicsCommandList7> commandList) noxnd;
	private:
		D3D12_VIEWPORT viewport;
	};
}