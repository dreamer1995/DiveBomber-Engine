#include "RenderPass.h"
#include "..\..\Graphics.h"
#include "..\..\Resource\RenderTarget.h"
#include "..\..\Resource\DepthStencil.h"

namespace DiveBomber::DEResource
{
	class RenderTargetAsShaderResourceView;
}

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DEResource;

	RenderPass::RenderPass(std::vector<std::shared_ptr<Pass>> inputPasses,
		std::shared_ptr<DEResource::RenderTarget> inputRenderTarget,
		std::shared_ptr<DEResource::DepthStencil> inputDepthStencil)
		:
		Pass(inputPasses),
		renderTarget(inputRenderTarget),
		depthStencil(inputDepthStencil)
	{
	}

	void RenderPass::SetRenderTarget(const std::shared_ptr<RenderTarget> inputRenderTarget) noexcept
	{
		renderTarget = inputRenderTarget;
	}

	void RenderPass::SetDepthStencil(const std::shared_ptr<DepthStencil> inputDepthStencil) noexcept
	{
		depthStencil = inputDepthStencil;
	}

	void RenderPass::Execute() noxnd
	{
		if (depthStencil != nullptr)
		{
			renderTarget->BindTarget(depthStencil);
		}
		else
		{
			renderTarget->BindTarget();
		}

		FLOAT clearColor[] = ClearMainRTColor;
		Graphics::GetInstance().GetGraphicsCommandList()->ClearRenderTargetView(renderTarget->GetRTVCPUDescriptorHandle(), clearColor, 0, nullptr);
	}
}
