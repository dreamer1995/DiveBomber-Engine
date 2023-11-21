#include "OpaqueGBufferPass.h"
#include "..\..\Graphics.h"
#include "..\..\Object\Object.h"
#include "..\..\Component\Camera.h"

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DEObject;
	using namespace Component;

	OpaqueGBufferPass::OpaqueGBufferPass(std::vector<std::shared_ptr<Pass>> inputPasses,
		std::shared_ptr<DEResource::RenderTarget> inputRenderTarget,
		std::shared_ptr<DEResource::DepthStencil> inputDepthStencil)
		:
		RenderPass(inputPasses, inputRenderTarget, inputDepthStencil)
	{
	}

	void OpaqueGBufferPass::SubmitObject(std::shared_ptr<DEObject::Object> object) noexcept
	{
		objects.emplace_back(object);
	}

	void OpaqueGBufferPass::Execute() noxnd
	{
		RenderPass::Execute();

		Graphics::GetInstance().GetCamera()->Bind();
		for (std::shared_ptr<DEObject::Object>& object : objects)
		{
			object->Bind();
		}

		objects.clear();
	}
}
