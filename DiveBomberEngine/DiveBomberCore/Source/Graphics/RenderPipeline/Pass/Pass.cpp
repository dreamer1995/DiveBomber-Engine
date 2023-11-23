#pragma once
#include "Pass.h"
#include "..\..\Resource\ShaderInputable\ShaderInputable.h"

namespace DiveBomber::RenderPipeline
{
	using namespace DEResource;

	std::vector<std::shared_ptr<Pass>> Pass::GetPreviousPass() noexcept
	{
		return prevPasses;
	}

	void Pass::SetPreviousPass(const std::vector<std::shared_ptr<Pass>> inputPasses) noexcept
	{
		prevPasses = inputPasses;
	}

	void Pass::AddPreviousPass(const std::shared_ptr<Pass> inputPass) noexcept
	{
		prevPasses.emplace_back(inputPass);
	}

	void Pass::ClearPreviousPass() noexcept
	{
		prevPasses.clear();
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