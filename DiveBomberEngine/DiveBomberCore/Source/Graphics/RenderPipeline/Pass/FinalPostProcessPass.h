#pragma once
#include "ComputePass.h"

#include <memory>

namespace DiveBomber::DEResource
{
	class PipelineStateObject;
	class ShaderInputable;
}

namespace DiveBomber::Component
{
	class Material;
}

namespace DiveBomber::RenderPipeline
{
	class FinalPostProcessPass : public ComputePass
	{
	public:
		FinalPostProcessPass(std::vector<std::shared_ptr<Pass>> inputPasses,
			std::shared_ptr<DEResource::UnorderedAccessBuffer> inputTarget);

		virtual void Execute() noxnd override;

	private:
		std::shared_ptr<Component::Material> material;
		std::shared_ptr<DEResource::PipelineStateObject> pso;
	};
}