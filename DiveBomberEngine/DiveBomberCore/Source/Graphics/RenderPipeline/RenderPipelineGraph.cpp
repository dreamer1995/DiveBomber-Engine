#include "RenderPipelineGraph.h"

#include "..\Graphics.h"
#include "..\Component\Camera.h"
#include "..\BindableObject\RenderTarget.h"
#include "..\BindableObject\DepthStencil.h"

#include <iostream>

namespace DiveBomber::RenderPipeline
{
	using namespace DX;
	using namespace DEGraphics;
	using namespace Component;

	RenderPipelineGraph::RenderPipelineGraph()
	{
	}

	RenderPipelineGraph::~RenderPipelineGraph()
	{
	}

	void RenderPipelineGraph::Bind(DEGraphics::Graphics& gfx) noxnd
	{
		gfx.BindShaderDescriptorHeaps();

		gfx.GetCurrentBackBuffer()->BindTarget(gfx, gfx.GetMainDS());

	}
}
