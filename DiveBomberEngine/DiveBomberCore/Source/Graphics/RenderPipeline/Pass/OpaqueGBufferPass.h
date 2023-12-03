#pragma once
#include "RenderPass.h"
#include "..\..\GraphicsHeader.h"

namespace DiveBomber::DEResource
{
	class RenderTargetAsShaderResourceView;
}

namespace DiveBomber::RenderPipeline
{
	class OpaqueGBufferPass final : public RenderPass
	{
	public:
		OpaqueGBufferPass(std::shared_ptr<DEResource::RenderTarget> inputRenderTarget,
			std::shared_ptr<DEResource::DepthStencil> inputDepthStencil);

		void Execute() noxnd override;

	private:
		std::shared_ptr<DEResource::RenderTargetAsShaderResourceView> baseColorBuffer;
		std::shared_ptr<DEResource::RenderTargetAsShaderResourceView> roughAOShadowSMIDBuffer;
		std::shared_ptr<DEResource::RenderTargetAsShaderResourceView> normalBuffer;
		std::shared_ptr<DEResource::RenderTargetAsShaderResourceView> customDataBuffer;

		std::vector<std::shared_ptr<DEResource::RenderTarget>> GBufferSet;
	};
}