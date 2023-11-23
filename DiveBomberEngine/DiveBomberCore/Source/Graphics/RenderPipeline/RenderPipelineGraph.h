#pragma once
#include "..\..\Utility\Common.h"

#include <memory>
#include <vector>

namespace DiveBomber::DEResource
{
	class RenderTargetAsShaderResourceView;
	class UnorderedAccessBufferAsShaderResourceView;
	class RootSignature;
}

namespace DiveBomber::DEObject
{
	class Object;
}

namespace DiveBomber::RenderPipeline
{
	class Pass;
	class OpaqueGBufferPass;
	class FinalPostProcessPass;

	class RenderPipelineGraph
	{
	public:
		RenderPipelineGraph();
		~RenderPipelineGraph();

		void Render() noxnd;

		void SubmitObject(std::shared_ptr<DEObject::Object> inputObject);

		void AddPass(std::shared_ptr<Pass> pass);

		std::shared_ptr<DEResource::RenderTargetAsShaderResourceView> HDRTarget;
		std::shared_ptr<DEResource::UnorderedAccessBufferAsShaderResourceView> finalTarget;
		std::shared_ptr<DEResource::RootSignature> rootSignature;

		std::shared_ptr<OpaqueGBufferPass> opaqueGBufferPass;
		std::shared_ptr<FinalPostProcessPass> finalPostProcessPass;

		std::vector<std::shared_ptr<Pass>> renderPath;

	private:
		void SetRenderPasses() noxnd;
		void BuildRenderPath() noexcept;
		void RecursivePassesTree(const std::shared_ptr<Pass> inputNode) noexcept;
	};
}