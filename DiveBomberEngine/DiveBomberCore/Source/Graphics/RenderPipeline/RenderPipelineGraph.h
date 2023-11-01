#pragma once
#include "..\..\Utility\Common.h"

#include <memory>
#include <vector>

namespace DiveBomber::BindableObject
{
	class RenderTargetAsShaderResourceView;
}

namespace DiveBomber::DrawableObject
{
	class Drawable;
	class FullScreenPlane;
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
		std::shared_ptr<BindableObject::RenderTargetAsShaderResourceView> HDRTarget;
	};
}