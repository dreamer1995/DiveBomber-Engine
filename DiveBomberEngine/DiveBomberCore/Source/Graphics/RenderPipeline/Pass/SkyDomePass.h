#pragma once
#include "RenderPass.h"

namespace DiveBomber::DEResource
{
	class IndexBuffer;
	class PipelineStateObject;
	class ShaderInputable;
}

namespace DiveBomber::Component
{
	class Mesh;
	class Material;
}

namespace DiveBomber::RenderPipeline
{
	class SkyDomePass final : public RenderPass
	{
	public:
		SkyDomePass(std::shared_ptr<DEResource::RenderTarget> inputRenderTarget);

		void Execute() noxnd override;

	private:
		std::shared_ptr<Component::Material> material;
		std::shared_ptr<Component::Mesh> mesh;
		std::shared_ptr<DEResource::PipelineStateObject> pso;
	};
}