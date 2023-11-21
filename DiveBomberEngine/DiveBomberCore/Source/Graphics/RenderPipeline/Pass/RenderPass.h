#pragma once
#include "Pass.h"
#include <memory>

namespace DiveBomber::DEResource
{
	class RenderTarget;
	class DepthStencil;
}

namespace DiveBomber::RenderPipeline
{
	class RenderPass : public Pass
	{
	public:
		RenderPass(std::vector<std::shared_ptr<Pass>> inputPasses,
			std::shared_ptr<DEResource::RenderTarget> inputRenderTarget,
			std::shared_ptr<DEResource::DepthStencil> inputDepthStencil = nullptr);

		virtual void Execute() noxnd override;
		void SetRenderTarget(const std::shared_ptr<DEResource::RenderTarget> inputRenderTarget) noexcept;
		void SetDepthStencil(const std::shared_ptr<DEResource::DepthStencil> inputDepthStencil) noexcept;

	protected:
		std::shared_ptr<DEResource::RenderTarget> renderTarget;
		std::shared_ptr<DEResource::DepthStencil> depthStencil;
	};
}