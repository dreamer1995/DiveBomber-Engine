#pragma once
#include "Pass.h"
#include <memory>
#include <vector>

namespace DiveBomber::GraphicResource
{
	class RenderTarget;
	class DepthStencil;
}

namespace DiveBomber::DEObject
{
	class Object;
}

namespace DiveBomber::RenderPipeline
{
	class RenderPass : public Pass
	{
	public:
		RenderPass(std::string inputName, std::shared_ptr<GraphicResource::RenderTarget> inputRenderTarget,
			std::shared_ptr<GraphicResource::DepthStencil> inputDepthStencil = nullptr);

		virtual void Execute() noxnd override;
		void SetRenderTarget(const std::shared_ptr<GraphicResource::RenderTarget> inputRenderTarget) noexcept;
		void SetDepthStencil(const std::shared_ptr<GraphicResource::DepthStencil> inputDepthStencil) noexcept;

		void SubmitObject(std::shared_ptr<DEObject::Object> object) noexcept;

	protected:
		std::shared_ptr<GraphicResource::RenderTarget> renderTarget;
		std::shared_ptr<GraphicResource::DepthStencil> depthStencil;
		std::vector<std::shared_ptr<DEObject::Object>> objects;
	};
}