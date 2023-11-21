#pragma once
#include "Pass.h"
#include "..\..\Resource\ShaderInputable\ShaderInputable.h"

namespace DiveBomber::RenderPipeline
{
	using namespace DEResource;

	Pass::Pass(std::vector<std::shared_ptr<Pass>> inputPasses)
		:
		prevPasses(inputPasses)
	{
	}

	void Pass::SetPreviousPass(const std::vector<std::shared_ptr<Pass>> inputPasses) noexcept
	{
		prevPasses = inputPasses;
	}

	void Pass::SetTexture(const std::shared_ptr<ShaderInputable> inputResource, UINT slot) noexcept
	{
		inputTexturesMap[slot] = inputResource;
	}

	void Pass::SetConstant(const std::shared_ptr<ShaderInputable> inputResource, UINT slot) noexcept
	{
		inputConstantsMap[slot] = inputResource;
	}
}