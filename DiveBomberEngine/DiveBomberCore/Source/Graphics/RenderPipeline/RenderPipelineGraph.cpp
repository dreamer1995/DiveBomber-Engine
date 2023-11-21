#include "RenderPipelineGraph.h"

#include "..\Graphics.h"
#include "..\DX\ShaderManager.h"
#include "..\DX\Commandlist.h"
#include "..\DX\GlobalResourceManager.h"
#include "..\Resource\Bindable\RootSignature.h"

#include "..\Resource\ShaderInputable\RenderTargetAsShaderResourceView.h"
#include "..\Resource\ShaderInputable\UnorderedAccessBufferAsShaderResourceView.h"

#include "Pass\OpaqueGBufferPass.h"
#include "Pass\FinalPostProcessPass.h"

#include "..\Object\Object.h"
#include "..\Object\UAVPass.h"

#include <iostream>

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DX;
	using namespace DEResource;
	using namespace DEObject;

	RenderPipelineGraph::RenderPipelineGraph()
	{
		HDRTarget = std::make_shared<RenderTargetAsShaderResourceView>(
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			DXGI_FORMAT_R32G32B32A32_FLOAT
			);

		std::vector<std::shared_ptr<Pass>> lastPasses;
		opaqueGBufferPass = std::make_shared<OpaqueGBufferPass>(lastPasses, HDRTarget, Graphics::GetInstance().GetMainDS());
		
		UAVTarget = std::make_shared<UnorderedAccessBufferAsShaderResourceView>(
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			DXGI_FORMAT_R8G8B8A8_UNORM);

		lastPasses = { opaqueGBufferPass };
		finalPostProcessPass = std::make_shared<FinalPostProcessPass>(lastPasses, UAVTarget->GetUAVPointer());

		rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");
	}

	RenderPipelineGraph::~RenderPipelineGraph()
	{
	}

	void RenderPipelineGraph::SetRenderQueue(std::shared_ptr<Object> inputObject)
	{
		opaqueGBufferPass->SubmitObject(inputObject);
	}

	void RenderPipelineGraph::AddPass(std::shared_ptr<Pass> pass)
	{
		passes.emplace_back(pass);
	}

	void RenderPipelineGraph::SetRenderPasses() noxnd
	{
		AddPass(opaqueGBufferPass);

		finalPostProcessPass->SetTexture(HDRTarget, 0u);
		finalPostProcessPass->SetTexture(UAVTarget->GetUAVPointer(), 1u);
		AddPass(finalPostProcessPass);
	}

	void RenderPipelineGraph::Render() noxnd
	{
		Graphics::GetInstance().BindShaderDescriptorHeaps();
		rootSignature->Bind();

		SetRenderPasses();

		for (std::shared_ptr<Pass>& pass : passes)
		{
			pass->Execute();
		}

		Graphics::GetInstance().GetCommandList()->CopyResource(
			Graphics::GetInstance().GetCurrentBackBuffer()->GetRenderTargetBuffer(),
			UAVTarget->GetUAVPointer()->GetUnorderedAccessBuffer());

		passes.clear();

		ShaderManager::GetInstance().ResetAllShaderDirtyState();
	}
}
