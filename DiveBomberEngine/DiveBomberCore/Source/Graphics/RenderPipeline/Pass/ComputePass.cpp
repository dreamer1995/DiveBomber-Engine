#include "ComputePass.h"
#include "..\..\GraphicResource\ShaderInputable\UnorderedAccessBuffer.h"
#include <iostream>

namespace DiveBomber::RenderPipeline
{
	using namespace GraphicResource;

	ComputePass::ComputePass(std::string inputName, std::shared_ptr<UnorderedAccessBuffer> inputTarget)
		:
		Pass(inputName),
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
