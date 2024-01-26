#pragma once
#include "RenderPass.h"

namespace DiveBomber::DEResource
{
	class IndexBuffer;
	class PipelineStateObject;
	class ShaderInputable;
	class ConstantTransformBuffer;
}

namespace DiveBomber::DEComponent
{
	class Mesh;
	class Material;
}

namespace DiveBomber::DEObject
{
	class Object;
}

namespace DiveBomber::RenderPipeline
{
	class SkyDomePass final : public RenderPass
	{
	public:
		SkyDomePass(std::shared_ptr<DEResource::RenderTarget> inputRenderTarget,
			std::shared_ptr<DEResource::DepthStencil> inputDepthStencil);

		void Execute() noxnd override;

	private:
		std::shared_ptr<DEComponent::Material> material;
		std::shared_ptr<DEComponent::Mesh> mesh;
		std::shared_ptr<DEResource::PipelineStateObject> pso;

		std::shared_ptr<DEObject::Object> skyDomeAnchor;
	};
}