#include "ComputePass.h"
#include "..\..\Resource\ShaderInputable\UnorderedAccessBuffer.h"
#include <iostream>

namespace DiveBomber::RenderPipeline
{
	using namespace DEResource;

	ComputePass::ComputePass(std::shared_ptr<UnorderedAccessBuffer> inputTarget)
		:
		uavTarget(inputTarget)
	{
	}

	void ComputePass::SetUnorderedAccessBuffer(const std::shared_ptr<UnorderedAccessBuffer> inputTarget) noexcept
	{
		uavTarget = inputTarget;
	}

	void ComputePass::Execute() noxnd
	{
		uavTarget->BindAsTarget();
	}
}
