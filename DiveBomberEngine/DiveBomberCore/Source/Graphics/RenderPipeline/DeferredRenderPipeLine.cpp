#include "DeferredRenderPipeLine.h"

#include "..\Graphics.h"
#include "..\DX\Commandlist.h"
#include "..\DX\GlobalResourceManager.h"
#include "..\Resource\Bindable\RootSignature.h"

#include "..\Resource\ShaderInputable\RenderTargetAsShaderResourceView.h"
#include "..\Resource\ShaderInputable\UnorderedAccessBufferAsShaderResourceView.h"

#include "Pass\OpaqueGBufferPass.h"
#include "Pass\FinalPostProcessPass.h"
#include "Pass\SkyDomePass.h"

#include "..\Object\Object.h"

#include <iostream>

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DX;
	using namespace DEResource;
	using namespace DEObject;

	DeferredRenderPipeLine::DeferredRenderPipeLine()
		:
		RenderPipelineGraph("DeferredRenderPipeLine")
	{
		HDRTarget = std::make_shared<RenderTargetAsShaderResourceView>(
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			DXGI_FORMAT_R32G32B32A32_FLOAT
			);

		opaqueGBufferPass = std::make_shared<OpaqueGBufferPass>(HDRTarget, Graphics::GetInstance().GetMainDS());

		//skyDomePass = std::make_shared<SkyDomePass>(HDRTarget, Graphics::GetInstance().GetMainDS());
		
		finalTarget = std::make_shared<UnorderedAccessBufferAsShaderResourceView>(
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			DXGI_FORMAT_R8G8B8A8_UNORM);

		finalPostProcessPass = std::make_shared<FinalPostProcessPass>(finalTarget->GetUAVPointer());

		rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");
	}

	DeferredRenderPipeLine::~DeferredRenderPipeLine()
	{
	}

	void DeferredRenderPipeLine::SubmitObject(std::shared_ptr<Object> inputObject)
	{
		opaqueGBufferPass->SubmitObject(inputObject);
	}

	void DeferredRenderPipeLine::SetRenderPasses() noxnd
	{
		std::shared_ptr<Pass> currentPass = opaqueGBufferPass;

		//currentPass = skyDomePass->LinkPass(passesTree, currentPass);

		finalPostProcessPass->SetTexture(HDRTarget, 0u);
		finalPostProcessPass->SetTexture(finalTarget->GetUAVPointer(), 1u);
		currentPass = finalPostProcessPass->LinkPass(passesTree, currentPass);
	}

	void DeferredRenderPipeLine::Render() noxnd
	{
		Graphics::GetInstance().BindShaderDescriptorHeaps();
		rootSignature->Bind();

		SetRenderPasses();
		BuildRenderPath(finalPostProcessPass);

		for (int i = (int)renderPath.size() - 1; i > -1; i--)
		{
			renderPath[i]->Execute();
		}

		static bool first = true;
		if (first)
		{
			for (int i = (int)renderPath.size() - 1; i > -1; i--)
			{
				std::cout << renderPath[i]->GetName() << std::endl;
			}
			first = false;
		}

		Graphics::GetInstance().GetCommandList()->CopyResource(
			Graphics::GetInstance().GetCurrentBackBuffer()->GetRenderTargetBuffer(),
			finalTarget->GetUAVPointer()->GetUnorderedAccessBuffer());

		PostRender();
	}

	void DeferredRenderPipeLine::PostRender() noxnd
	{
		RenderPipelineGraph::PostRender();
	}
}
