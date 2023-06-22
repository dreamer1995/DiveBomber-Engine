#include "DepthStencil.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	DepthStencil::DepthStencil(Graphics& gfx, UINT inputWidth, UINT inputHeight, DXGI_FORMAT inputFormat, UINT inputDepth)
	{
	}

	void DepthStencil::Bind(Graphics& gfx) noxnd
	{
		//GFX_THROW_INFO_ONLY(gfx.GetCommandList()->IASetPrimitiveTopology(type));
	}
}
