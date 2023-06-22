#pragma once
#include "Bindable.h"
#include "..\Graphics.h"

namespace DiveBomber::BindObj
{
	class RenderTarget final: public Bindable
	{
		// just for reference
		//enum class RenderTargetType
		//{
		//	RT_Default,
		//	RT_PreCalSimpleCube,
		//	RT_PreCalMipCube,
		//	RT_PreBRDFCoefficient,
		//	RT_GBuffer,
		//	RT_UVABuffer,
		//	RT_3D
		//};
	public:
		RenderTarget(DEGraphics::Graphics& gfx, UINT inputWidth, UINT inputHeight,
			DXGI_FORMAT inputFormat = DXGI_FORMAT_B8G8R8A8_UNORM, UINT inputDepth = 0);
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
	private:
		//what should I define type here???
	};
}