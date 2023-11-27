#include "OpaqueGBufferPass.h"
#include "..\..\Graphics.h"
#include "..\..\Component\Camera.h"
#include "..\..\Resource\RenderTarget.h"

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace Component;
	using namespace DEResource;

	OpaqueGBufferPass::OpaqueGBufferPass(std::shared_ptr<RenderTarget> inputRenderTarget,
		std::shared_ptr<DepthStencil> inputDepthStencil)
		:
		RenderPass("OpaqueGBufferPass", inputRenderTarget, inputDepthStencil)
	{
	}

	void OpaqueGBufferPass::Execute() noxnd
	{
		Graphics::GetInstance().GetCamera()->Bind();

		FLOAT clearColor[] = ClearMainRTColor;
		Graphics::GetInstance().GetGraphicsCommandList()->ClearRenderTargetView(renderTarget->GetRTVCPUDescriptorHandle(), clearColor, 0, nullptr);

		RenderPass::Execute();
	}
}
