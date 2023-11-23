#include "RenderPipelineGraph.h"

#include "..\DX\ShaderManager.h"

namespace DiveBomber::RenderPipeline
{
	using namespace DX;

	RenderPipelineGraph::RenderPipelineGraph(std::string inputName)
		:
		name(inputName)
	{
	}

	void RenderPipelineGraph::PostRender() noxnd
	{
		renderPath.clear();
		ShaderManager::GetInstance().ResetAllShaderDirtyState();
	}

	void RenderPipelineGraph::BuildRenderPath(const std::shared_ptr<Pass> finalPass) noexcept
	{
		renderPath.emplace_back(finalPass);
		RecursivePassesTree(passesTree[finalPass]);
		passesTree.clear();
	}

	void RenderPipelineGraph::RecursivePassesTree(const std::unordered_set<std::shared_ptr<Pass>> inputNode) noexcept
	{
		for (const std::shared_ptr<Pass>& currentPass : inputNode)
		{
			auto iterator = std::find(renderPath.begin(), renderPath.end(), currentPass);
			if (iterator != renderPath.end())
			{
				renderPath.erase(iterator);
			}
			renderPath.emplace_back(currentPass);
			RecursivePassesTree(passesTree[currentPass]);
		}
	}

	void RenderPipelineGraph::LinkPass(const std::shared_ptr<Pass> beforePass, const std::unordered_set<std::shared_ptr<Pass>> afterPasses) noexcept
	{
		passesTree[beforePass] = afterPasses;
	}

	void RenderPipelineGraph::LinkPass(const std::shared_ptr<Pass> beforePass, const std::shared_ptr<Pass> afterPass) noexcept
	{
		passesTree[beforePass] = { afterPass };
	}

	void RenderPipelineGraph::AddLinkPass(const std::shared_ptr<Pass> beforePass, const std::shared_ptr<Pass> afterPass) noexcept
	{
		passesTree[beforePass].emplace(afterPass);
	}
}
