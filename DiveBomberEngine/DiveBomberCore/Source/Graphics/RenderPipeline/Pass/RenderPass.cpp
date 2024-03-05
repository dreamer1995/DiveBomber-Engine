#include "RenderPass.h"
#include "..\..\Graphics.h"
#include "..\..\GraphicResource\RenderTarget.h"
#include "..\..\GraphicResource\DepthStencil.h"
#include "..\..\Object\Object.h"

namespace DiveBomber::GraphicResource
{
	class RenderTargetAsShaderResourceView;
}

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace GraphicResource;
	using namespace DEObject;

	RenderPass::RenderPass(std::string inputName, std::shared_ptr<GraphicResource::RenderTarget> inputRenderTarget,
		std::shared_ptr<GraphicResource::DepthStencil> inputDepthStencil)
		:
		Pass(inputName),
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
		
		for (std::shared_ptr<DEObject::Object>& object : objects)
		{
			object->Render();
		}

		objects.clear();
	}
}
