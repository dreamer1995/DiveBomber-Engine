#pragma once
#include "..\..\Utility\Common.h"

#include <memory>
#include <vector>

namespace DiveBomber::BindableObject
{
	class RenderTargetAsShaderResourceView;
	class UnorderedAccessBufferAsShaderResourceView;
	class RootSignature;
}

namespace DiveBomber::DrawableObject
{
	class Drawable;
	class FullScreenPlane;
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

		void SetRenderQueue(std::shared_ptr<DrawableObject::Drawable> inputObject);
	private:
		std::vector<std::shared_ptr<DrawableObject::Drawable>> drawableObjects;

		std::shared_ptr<DrawableObject::FullScreenPlane> fullScreenPlane;
		std::shared_ptr<DrawableObject::UAVPass> uavPass;
		std::shared_ptr<BindableObject::RenderTargetAsShaderResourceView> HDRTarget;
		std::shared_ptr<BindableObject::UnorderedAccessBufferAsShaderResourceView> UAVTarget;
		std::shared_ptr<BindableObject::RootSignature> rootSignature;
	};
}