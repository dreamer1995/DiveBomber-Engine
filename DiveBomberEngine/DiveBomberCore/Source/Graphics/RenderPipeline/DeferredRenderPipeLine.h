#pragma once
#include "RenderPipelineGraph.h"
#include "..\..\Utility\Common.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace DiveBomber::GraphicResource
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
	class SkyDomePass;

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
		std::shared_ptr<GraphicResource::RenderTargetAsShaderResourceView> HDRTarget;
		std::shared_ptr<GraphicResource::UnorderedAccessBufferAsShaderResourceView> finalTarget;
		std::shared_ptr<GraphicResource::RootSignature> rootSignature;

		std::shared_ptr<OpaqueGBufferPass> opaqueGBufferPass;
		std::shared_ptr<FinalPostProcessPass> finalPostProcessPass;
		std::shared_ptr<SkyDomePass> skyDomePass;
	};
}