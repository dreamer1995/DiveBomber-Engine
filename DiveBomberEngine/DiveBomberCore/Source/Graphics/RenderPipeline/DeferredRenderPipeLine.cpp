#include "DeferredRenderPipeLine.h"

#include "..\Graphics.h"
#include "..\DX\Commandlist.h"
#include "..\DX\GlobalResourceManager.h"
#include "..\Resource\Bindable\RootSignature.h"

#include "..\Resource\ShaderInputable\RenderTargetAsShaderResourceView.h"
#include "..\Resource\ShaderInputable\UnorderedAccessBufferAsShaderResourceView.h"

#include "Pass\OpaqueGBufferPass.h"
#include "Pass\FinalPostProcessPass.h"

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

		std::vector<std::shared_ptr<Pass>> lastPasses;
		opaqueGBufferPass = std::make_shared<OpaqueGBufferPass>(HDRTarget, Graphics::GetInstance().GetMainDS());
		
		finalTarget = std::make_shared<UnorderedAccessBufferAsShaderResourceView>(
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			DXGI_FORMAT_R8G8B8A8_UNORM);

		lastPasses = { opaqueGBufferPass };
		finalPostProcessPass = std::make_shared<FinalPostProcessPass>(finalTarget->GetUAVPointer());

		rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");

		testPass01 = std::make_shared<ComputePass>("testPass01", finalTarget->GetUAVPointer());
		testPass02 = std::make_shared<ComputePass>("testPass02", finalTarget->GetUAVPointer());
		testPass03 = std::make_shared<ComputePass>("testPass03", finalTarget->GetUAVPointer());
		testPass04 = std::make_shared<ComputePass>("testPass04", finalTarget->GetUAVPointer());
		testPass05 = std::make_shared<ComputePass>("testPass05", finalTarget->GetUAVPointer());
		testPass06 = std::make_shared<ComputePass>("testPass06", finalTarget->GetUAVPointer());
		testPass07 = std::make_shared<ComputePass>("testPass07", finalTarget->GetUAVPointer());
		testPass08 = std::make_shared<ComputePass>("testPass08", finalTarget->GetUAVPointer());
		testPass09 = std::make_shared<ComputePass>("testPass09", finalTarget->GetUAVPointer());
		testPass10 = std::make_shared<ComputePass>("testPass10", finalTarget->GetUAVPointer());
		testPass11 = std::make_shared<ComputePass>("testPass11", finalTarget->GetUAVPointer());
		testPass12 = std::make_shared<ComputePass>("testPass12", finalTarget->GetUAVPointer());
		testPass13 = std::make_shared<ComputePass>("testPass13", finalTarget->GetUAVPointer());
		testPass14 = std::make_shared<ComputePass>("testPass14", finalTarget->GetUAVPointer());
		testPass15 = std::make_shared<ComputePass>("testPass15", finalTarget->GetUAVPointer());
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
		std::shared_ptr<Pass> currentPass = testPass01;

		testPass02->LinkPass(passesTree, currentPass);
		testPass03->LinkPass(passesTree, currentPass);

		currentPass = testPass04->LinkPass(passesTree, { testPass02,testPass03 });

		testPass05->LinkPass(passesTree, currentPass);
		testPass06->LinkPass(passesTree, currentPass);

		currentPass = testPass07->LinkPass(passesTree, { testPass05,testPass06 });

		testPass09->LinkPass(passesTree, testPass08);
		testPass10->LinkPass(passesTree, testPass08);

		currentPass = testPass11->LinkPass(passesTree, { testPass09,testPass10 });

		testPass12->LinkPass(passesTree, currentPass);
		testPass13->LinkPass(passesTree, currentPass);

		testPass14->LinkPass(passesTree, { testPass12,testPass13 });

		currentPass = testPass15->LinkPass(passesTree, { testPass07,testPass14 });

		currentPass = opaqueGBufferPass->LinkPass(passesTree, currentPass);
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
