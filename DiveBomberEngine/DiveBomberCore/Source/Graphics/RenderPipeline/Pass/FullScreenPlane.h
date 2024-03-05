#pragma once
#include "RenderPass.h"

namespace DiveBomber::GraphicResource
{
	class IndexBuffer;
	class PipelineStateObject;
	class ShaderInputable;
}

namespace DiveBomber::DEComponent
{
	class Mesh;
	class Material;
}

namespace DiveBomber::RenderPipeline
{
	class FullScreenPlane final : public RenderPass
	{
	public:
		FullScreenPlane(std::string inputName, std::shared_ptr<GraphicResource::RenderTarget> inputRenderTarget);

		void Execute() noxnd override;

	private:
		std::shared_ptr<DEComponent::Material> material;
		std::shared_ptr<DEComponent::Mesh> mesh;
		std::shared_ptr<GraphicResource::PipelineStateObject> pso;
	};
}