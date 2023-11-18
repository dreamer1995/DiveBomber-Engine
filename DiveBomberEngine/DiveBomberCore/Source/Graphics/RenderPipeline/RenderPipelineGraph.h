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
	class Drawable;
	class UAVPass;
}

namespace DiveBomber::RenderPipeline
{
	class RenderPipelineGraph
	{
	public:
		RenderPipelineGraph();
		~RenderPipelineGraph();

		void Bind() noxnd;

		void SetRenderQueue(std::shared_ptr<DEObject::Drawable> inputObject);
	private:
		std::vector<std::shared_ptr<DEObject::Drawable>> drawableObjects;

		std::shared_ptr<DEObject::UAVPass> uavPass;
		std::shared_ptr<DEResource::RenderTargetAsShaderResourceView> HDRTarget;
		std::shared_ptr<DEResource::UnorderedAccessBufferAsShaderResourceView> UAVTarget;
		std::shared_ptr<DEResource::RootSignature> rootSignature;
	};
}