#pragma once
#include "..\..\..\Utility\Common.h"
#include "..\..\..\Window\DEWindows.h"

#include <memory>
#include <vector>
#include <unordered_map>

namespace DiveBomber::DEResource
{
	class ShaderInputable;
}

namespace DiveBomber::RenderPipeline
{
	class Pass
	{
	public:
		Pass(std::vector<std::shared_ptr<Pass>> inputPasses);

		virtual ~Pass() = default;

		virtual void Execute() noxnd = 0;
		void SetPreviousPass(const std::vector<std::shared_ptr<Pass>> inputPasses) noexcept;

		void SetTexture(const std::shared_ptr<DEResource::ShaderInputable> inputResource, UINT slot) noexcept;
		void SetConstant(const std::shared_ptr<DEResource::ShaderInputable> inputResource, UINT slot) noexcept;

	protected:
		std::vector<std::shared_ptr<Pass>> prevPasses;
		std::unordered_map<UINT, std::shared_ptr<DEResource::ShaderInputable>> inputTexturesMap;
		std::unordered_map<UINT, std::shared_ptr<DEResource::ShaderInputable>> inputConstantsMap;
	};
}