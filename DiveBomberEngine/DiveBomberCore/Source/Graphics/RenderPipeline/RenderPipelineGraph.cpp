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
		opaqueGBufferPass = std::make_shared<OpaqueGBufferPass>(HDRTarget, Graphics::GetInstance().GetMainDS());
		
		finalTarget = std::make_shared<UnorderedAccessBufferAsShaderResourceView>(
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			DXGI_FORMAT_R8G8B8A8_UNORM);

		lastPasses = { opaqueGBufferPass };
		finalPostProcessPass = std::make_shared<FinalPostProcessPass>(finalTarget->GetUAVPointer());

		rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");
	}

	RenderPipelineGraph::~RenderPipelineGraph()
	{
	}

	void RenderPipelineGraph::SubmitObject(std::shared_ptr<Object> inputObject)
	{
		opaqueGBufferPass->SubmitObject(inputObject);
	}

	void RenderPipelineGraph::SetRenderPasses() noxnd
	{
		finalPostProcessPass->SetTexture(HDRTarget, 0u);
		finalPostProcessPass->SetTexture(finalTarget->GetUAVPointer(), 1u);
		finalPostProcessPass->AddPreviousPass(opaqueGBufferPass);
	}

	void RenderPipelineGraph::BuildRenderPath() noexcept
	{
		std::vector<std::shared_ptr<Pass>> currentPasses = finalPostProcessPass->GetPreviousPass();
		renderPath.emplace_back(finalPostProcessPass);
		RecursivePassesTree(finalPostProcessPass);
	}

	void RenderPipelineGraph::RecursivePassesTree(const std::shared_ptr<Pass> inputNode) noexcept
	{
		std::vector<std::shared_ptr<Pass>> currentPasses = inputNode->GetPreviousPass();
		for (std::shared_ptr<Pass>& currentPass : currentPasses)
		{
			auto iterator = std::find(renderPath.begin(), renderPath.end(), currentPass);
			if (iterator != renderPath.end())
			{
				renderPath.erase(iterator);
			}
			renderPath.emplace_back(currentPass);
			RecursivePassesTree(currentPass);
		}
	}

	void RenderPipelineGraph::Render() noxnd
	{
		Graphics::GetInstance().BindShaderDescriptorHeaps();
		rootSignature->Bind();

		SetRenderPasses();
		BuildRenderPath();

		for (int i = (int)renderPath.size() - 1; i > -1; i--)
		{
			renderPath[i]->Execute();
			renderPath[i]->ClearPreviousPass();
		}

		Graphics::GetInstance().GetCommandList()->CopyResource(
			Graphics::GetInstance().GetCurrentBackBuffer()->GetRenderTargetBuffer(),
			finalTarget->GetUAVPointer()->GetUnorderedAccessBuffer());

		renderPath.clear();

		ShaderManager::GetInstance().ResetAllShaderDirtyState();
	}
}
