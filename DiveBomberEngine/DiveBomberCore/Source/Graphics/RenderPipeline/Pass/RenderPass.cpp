#include "RenderPass.h"
#include "..\..\Graphics.h"
#include "..\..\Resource\RenderTarget.h"
#include "..\..\Resource\DepthStencil.h"
#include "..\..\Object\Object.h"

namespace DiveBomber::DEResource
{
	class RenderTargetAsShaderResourceView;
}

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DEResource;
	using namespace DEObject;

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

	void RenderPass::SubmitObject(std::shared_ptr<DEObject::Object> object) noexcept
	{
		objects.emplace_back(object);
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
		
		for (std::shared_ptr<DEObject::Object>& object : objects)
		{
			object->Bind();
		}

		objects.clear();
	}
}
