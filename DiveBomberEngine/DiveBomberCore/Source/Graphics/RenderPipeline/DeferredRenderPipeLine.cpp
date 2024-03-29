#include "DeferredRenderPipeLine.h"

#include "..\Graphics.h"
#include "..\DX\Commandlist.h"
#include "..\DX\GlobalResourceManager.h"
#include "..\GraphicResource\Bindable\RootSignature.h"

#include "..\GraphicResource\ShaderInputable\RenderTargetAsShaderResourceView.h"
#include "..\GraphicResource\ShaderInputable\UnorderedAccessBufferAsShaderResourceView.h"

#include "Pass\OpaqueGBufferPass.h"
#include "Pass\FinalPostProcessPass.h"
#include "Pass\SkyDomePass.h"

#include "..\Object\Object.h"

#include <iostream>

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DX;
	using namespace GraphicResource;
	using namespace DEObject;

	DeferredRenderPipeLine::DeferredRenderPipeLine()
		:
		RenderPipelineGraph("DeferredRenderPipeLine")
	{
		auto HDRDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT,
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		HDRTarget = std::make_shared<RenderTargetAsShaderResourceView>(
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			std::move(HDRDesc)
			);

		opaqueGBufferPass = std::make_shared<OpaqueGBufferPass>(HDRTarget, Graphics::GetInstance().GetMainDS());

		skyDomePass = std::make_shared<SkyDomePass>(HDRTarget, Graphics::GetInstance().GetMainDS());
		
		auto finalTargetDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		finalTarget = std::make_shared<UnorderedAccessBufferAsShaderResourceView>(
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			finalTargetDesc);

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
		if (Graphics::GetInstance().CheckResolutionDirty())
		{
			HDRTarget->Resize(Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight());
			finalTarget->Resize(Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight());
		}

		std::shared_ptr<Pass> currentPass = opaqueGBufferPass;

		currentPass = skyDomePass->LinkPass(passesTree, currentPass);

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

		// Set backbufferfor for UI Paint.
		D3D12_CPU_DESCRIPTOR_HANDLE backBufferHandle = Graphics::GetInstance().GetCurrentBackBuffer()->GetRTVCPUDescriptorHandle();
		Graphics::GetInstance().GetGraphicsCommandList()->OMSetRenderTargets(1, &backBufferHandle, FALSE, nullptr);
		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(
			Graphics::GetInstance().GetCurrentBackBuffer()->GetRenderTargetBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, true);

		PostRender();
	}

	void DeferredRenderPipeLine::PostRender() noxnd
	{
		RenderPipelineGraph::PostRender();
	}
}
