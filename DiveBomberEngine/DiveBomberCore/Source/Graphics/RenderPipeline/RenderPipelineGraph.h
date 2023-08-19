#pragma once
#include "..\..\Utility\Common.h"

#include <memory>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::RenderPipeline
{
	class RenderPipelineGraph
	{
	public:
		RenderPipelineGraph();
		~RenderPipelineGraph();

		void Bind(DEGraphics::Graphics& gfx) noxnd;
	private:
	};
}