#include "RenderTarget.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	RenderTarget::RenderTarget(Graphics& gfx, UINT inputWidth, UINT inputHeight, DXGI_FORMAT inputFormat, UINT inputDepth)
	{
	}

	void RenderTarget::Bind(Graphics& gfx) noxnd
	{
		//GFX_THROW_INFO_ONLY(gfx.GetCommandList()->IASetPrimitiveTopology(type));
	}
}
