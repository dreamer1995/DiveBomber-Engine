#pragma once
#include "ComputePass.h"
#include "..\..\GraphicResource\GraphicsResourceCommonInclude.h"

#include <memory>

namespace DiveBomber::GraphicResource
{
	class PipelineStateObject;
	class ShaderInputable;
}

namespace DiveBomber::DEComponent
{
	class Material;
}

namespace DiveBomber::RenderPipeline
{
	class FinalPostProcessPass final : public ComputePass
	{
	public:
		struct PostProcessData
		{
			DirectX::XMFLOAT2 invScreenSize;
		};

	public:
		FinalPostProcessPass(std::shared_ptr<GraphicResource::UnorderedAccessBuffer> inputTarget);

		virtual void Execute() noxnd override;

	private:
		std::shared_ptr<DEComponent::Material> material;
		std::shared_ptr<GraphicResource::PipelineStateObject> pso;
		PostProcessData postProcessCB;
		std::shared_ptr<GraphicResource::ConstantBufferInHeap<PostProcessData>> postProcessCBIndex;
	};
}