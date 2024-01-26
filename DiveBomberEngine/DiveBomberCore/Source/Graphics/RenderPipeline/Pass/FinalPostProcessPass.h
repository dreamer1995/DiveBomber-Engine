#pragma once
#include "ComputePass.h"

#include <memory>

namespace DiveBomber::DEResource
{
	class PipelineStateObject;
	class ShaderInputable;
}

namespace DiveBomber::DEComponent
{
	class Material;
}

namespace DiveBomber::RenderPipeline
{
	class FinalPostProcessPass final : public ComputePass
	{
	public:
		FinalPostProcessPass(std::shared_ptr<DEResource::UnorderedAccessBuffer> inputTarget);

		virtual void Execute() noxnd override;

	private:
		std::shared_ptr<DEComponent::Material> material;
		std::shared_ptr<DEResource::PipelineStateObject> pso;
	};
}