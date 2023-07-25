#pragma once
#include "..\GraphicsHeader.h"

namespace DiveBomber::DX
{
	class ScissorRects final
	{
	public:
		ScissorRects();
		[[nodiscard]] D3D12_RECT GetScissorRects() const noexcept;
		void Bind(wrl::ComPtr<ID3D12GraphicsCommandList2> commandList) noxnd;
	private:
		D3D12_RECT scissorRects;
	};
}