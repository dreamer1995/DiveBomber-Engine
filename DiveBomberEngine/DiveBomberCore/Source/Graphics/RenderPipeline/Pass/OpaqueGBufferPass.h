#pragma once
#include "RenderPass.h"

namespace DiveBomber::DEObject
{
	class Object;
}

#include <vector>
namespace DiveBomber::RenderPipeline
{
	class OpaqueGBufferPass final : public RenderPass
	{
	public:
		OpaqueGBufferPass(std::vector<std::shared_ptr<Pass>> inputPasses,
			std::shared_ptr<DEResource::RenderTarget> inputRenderTarget,
			std::shared_ptr<DEResource::DepthStencil> inputDepthStencil);

		void SubmitObject(std::shared_ptr<DEObject::Object> object) noexcept;

		void Execute() noxnd override;
	private:
		std::vector<std::shared_ptr<DEObject::Object>> objects;
	};
}