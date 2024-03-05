#pragma once
#include "..\..\..\Utility\Common.h"
#include "..\..\..\Window\DEWindows.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace DiveBomber::GraphicResource
{
	class ShaderInputable;
}

namespace DiveBomber::RenderPipeline
{
	class Pass : public std::enable_shared_from_this<Pass>
	{
	public:
		Pass(std::string inputName);

		virtual ~Pass() = default;

		virtual void Execute() noxnd = 0;

		void SetTexture(const std::shared_ptr<GraphicResource::ShaderInputable> inputResource, UINT slot) noexcept;
		void SetConstant(const std::shared_ptr<GraphicResource::ShaderInputable> inputResource, UINT slot) noexcept;

		[[nodiscard]] std::string GetName() const noexcept;

		std::shared_ptr<Pass> LinkPass(std::unordered_map<std::shared_ptr<Pass>, std::unordered_set<std::shared_ptr<Pass>>>& passesTree,
			const std::unordered_set<std::shared_ptr<Pass>> afterPasses) noexcept;
		std::shared_ptr<Pass> LinkPass(std::unordered_map<std::shared_ptr<Pass>, std::unordered_set<std::shared_ptr<Pass>>>& passesTree,
			const std::shared_ptr<Pass> afterPass) noexcept;
		std::shared_ptr<Pass> AddLinkPass(std::unordered_map<std::shared_ptr<Pass>, std::unordered_set<std::shared_ptr<Pass>>>& passesTree,
			const std::shared_ptr<Pass> afterPass) noexcept;
	protected:
		std::string name;

		std::unordered_map<UINT, std::shared_ptr<GraphicResource::ShaderInputable>> inputTexturesMap;
		std::unordered_map<UINT, std::shared_ptr<GraphicResource::ShaderInputable>> inputConstantsMap;
	};
}