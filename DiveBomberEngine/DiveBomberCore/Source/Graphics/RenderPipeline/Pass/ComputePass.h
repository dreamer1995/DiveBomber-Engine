#pragma once
#include "Pass.h"
#include <memory>

namespace DiveBomber::GraphicResource
{
	class UnorderedAccessBuffer;
}

namespace DiveBomber::RenderPipeline
{
	class ComputePass : public Pass
	{
	public:
		ComputePass(std::string inputName, std::shared_ptr<GraphicResource::UnorderedAccessBuffer> inputTarget);

		virtual void Execute() noxnd override;
		void SetUnorderedAccessBuffer(const std::shared_ptr<GraphicResource::UnorderedAccessBuffer> inputTarget) noexcept;

	protected:
		std::shared_ptr<GraphicResource::UnorderedAccessBuffer> uavTarget;
	};
}