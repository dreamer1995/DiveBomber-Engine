#pragma once
#include "RenderPass.h"

namespace DiveBomber::RenderPipeline
{
	class OpaqueGBufferPass final : public RenderPass
	{
	public:
		OpaqueGBufferPass(std::shared_ptr<DEResource::RenderTarget> inputRenderTarget,
			std::shared_ptr<DEResource::DepthStencil> inputDepthStencil);

		void Execute() noxnd override;
	};
}