#pragma once
#include "..\..\Utility\Common.h"

#include <memory>

namespace DiveBomber::RenderPipeline
{
	class RenderPipelineGraph
	{
	public:
		RenderPipelineGraph();
		~RenderPipelineGraph();

		void Bind() noxnd;
	private:
	};
}