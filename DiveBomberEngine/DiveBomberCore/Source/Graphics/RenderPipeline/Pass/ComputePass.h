#pragma once
#include "Pass.h"
#include <memory>

namespace DiveBomber::DEResource
{
	class UnorderedAccessBuffer;
}

namespace DiveBomber::RenderPipeline
{
	class ComputePass : public Pass
	{
	public:
		ComputePass(std::shared_ptr<DEResource::UnorderedAccessBuffer> inputTarget);

		virtual void Execute() noxnd override;
		void SetUnorderedAccessBuffer(const std::shared_ptr<DEResource::UnorderedAccessBuffer> inputTarget) noexcept;

	protected:
		std::shared_ptr<DEResource::UnorderedAccessBuffer> uavTarget;
	};
}