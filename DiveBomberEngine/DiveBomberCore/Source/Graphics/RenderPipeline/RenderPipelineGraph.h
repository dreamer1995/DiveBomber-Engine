#pragma once
#include "..\..\Utility\Common.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace DiveBomber::RenderPipeline
{
	class Pass;

	class RenderPipelineGraph
	{
	public:
		RenderPipelineGraph(std::string inputName);
		virtual ~RenderPipelineGraph() = default;

		virtual void Render() noxnd = 0;

	protected:
		virtual void SetRenderPasses() noxnd = 0;
		virtual void PostRender() noxnd;
		void BuildRenderPath(const std::shared_ptr<Pass> finalPass) noexcept;
		void RecursivePassesTree(const std::unordered_set<std::shared_ptr<Pass>> inputNode) noexcept;
		void LinkPass(const std::shared_ptr<Pass> beforePass, const std::unordered_set<std::shared_ptr<Pass>> afterPasses) noexcept;
		void LinkPass(const std::shared_ptr<Pass> beforePass, const std::shared_ptr<Pass> afterPass) noexcept;
		void AddLinkPass(const std::shared_ptr<Pass> beforePass, const std::shared_ptr<Pass> afterPass) noexcept;

	protected:
		std::string name;

		std::unordered_map<std::shared_ptr<Pass>, std::unordered_set<std::shared_ptr<Pass>>> passesTree;
		std::vector<std::shared_ptr<Pass>> renderPath;
	};
}