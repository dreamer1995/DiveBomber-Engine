#pragma once
#include "RenderPipelineGraph.h"
#include "..\..\Utility\Common.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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
	class OpaqueGBufferPass;
	class FinalPostProcessPass;
	class ComputePass;

	class DeferredRenderPipeLine final : public RenderPipelineGraph
	{
	public:
		DeferredRenderPipeLine();
		~DeferredRenderPipeLine();

		void Render() noxnd override;

		void SubmitObject(std::shared_ptr<DEObject::Object> inputObject);

	private:
		void SetRenderPasses() noxnd override;
		void PostRender() noxnd override;

	private:
		std::shared_ptr<DEResource::RenderTargetAsShaderResourceView> HDRTarget;
		std::shared_ptr<DEResource::UnorderedAccessBufferAsShaderResourceView> finalTarget;
		std::shared_ptr<DEResource::RootSignature> rootSignature;

		std::shared_ptr<OpaqueGBufferPass> opaqueGBufferPass;
		std::shared_ptr<FinalPostProcessPass> finalPostProcessPass;

		std::shared_ptr<ComputePass> testPass01;
		std::shared_ptr<ComputePass> testPass02;
		std::shared_ptr<ComputePass> testPass03;
		std::shared_ptr<ComputePass> testPass04;
		std::shared_ptr<ComputePass> testPass05;
		std::shared_ptr<ComputePass> testPass06;
		std::shared_ptr<ComputePass> testPass07;
		std::shared_ptr<ComputePass> testPass08;
		std::shared_ptr<ComputePass> testPass09;
		std::shared_ptr<ComputePass> testPass10;
		std::shared_ptr<ComputePass> testPass11;
		std::shared_ptr<ComputePass> testPass12;
		std::shared_ptr<ComputePass> testPass13;
		std::shared_ptr<ComputePass> testPass14;
		std::shared_ptr<ComputePass> testPass15;
	};
}