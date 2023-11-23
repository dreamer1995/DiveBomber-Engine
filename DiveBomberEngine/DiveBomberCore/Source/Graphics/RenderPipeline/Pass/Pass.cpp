#pragma once
#include "Pass.h"
#include "..\..\Resource\ShaderInputable\ShaderInputable.h"

namespace DiveBomber::RenderPipeline
{
	using namespace DEResource;

	Pass::Pass(std::string inputName)
		:
		name(inputName)
	{
	}

	void Pass::SetTexture(const std::shared_ptr<ShaderInputable> inputResource, UINT slot) noexcept
	{
		inputTexturesMap[slot] = inputResource;
	}

	void Pass::SetConstant(const std::shared_ptr<ShaderInputable> inputResource, UINT slot) noexcept
	{
		inputConstantsMap[slot] = inputResource;
	}

	std::string Pass::GetName() const noexcept
	{
		return name;
	}

	std::shared_ptr<Pass> Pass::LinkPass(std::unordered_map<std::shared_ptr<Pass>, std::unordered_set<std::shared_ptr<Pass>>>& passesTree,
		const std::unordered_set<std::shared_ptr<Pass>> afterPasses) noexcept
	{
		passesTree[shared_from_this()] = afterPasses;
		return shared_from_this();
	}

	std::shared_ptr<Pass> Pass::LinkPass(std::unordered_map<std::shared_ptr<Pass>, std::unordered_set<std::shared_ptr<Pass>>>& passesTree,
		const std::shared_ptr<Pass> afterPass) noexcept
	{
		passesTree[shared_from_this()] = { afterPass };
		return shared_from_this();
	}

	std::shared_ptr<Pass> Pass::AddLinkPass(std::unordered_map<std::shared_ptr<Pass>, std::unordered_set<std::shared_ptr<Pass>>>& passesTree,
		const std::shared_ptr<Pass> afterPass) noexcept
	{
		passesTree[shared_from_this()].emplace(afterPass);
		return shared_from_this();
	}
}