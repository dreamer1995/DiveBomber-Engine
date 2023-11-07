#include "RenderPipelineGraph.h"

#include "..\Graphics.h"
#include "..\DX\ShaderManager.h"
#include "..\DX\Commandlist.h"
#include "..\Component\Camera.h"
#include "..\BindableObject\RenderTarget.h"
#include "..\BindableObject\DepthStencil.h"
#include "..\BindableObject\RootSignature.h"

#include "..\BindableObject\RenderTargetAsShaderResourceView.h"
#include "..\BindableObject\UnorderedAccessBufferAsShaderResourceView.h"

#include "..\DrawableObject\UAVPass.h"

#include <iostream>

namespace DiveBomber::RenderPipeline
{
	using namespace DX;
	using namespace DEGraphics;
	using namespace Component;
	using namespace BindableObject;
	using namespace DrawableObject;

	RenderPipelineGraph::RenderPipelineGraph()
	{

		HDRTarget = std::make_shared<RenderTargetAsShaderResourceView>(
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			DXGI_FORMAT_R32G32B32A32_FLOAT
			);


		uavPass = std::make_shared<UAVPass>(L"PostProcessPass");
		UAVTarget = std::make_shared<UnorderedAccessBufferAsShaderResourceView>(
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			DXGI_FORMAT_R8G8B8A8_UNORM
		);
		uavPass->SetTexture(HDRTarget);
		uavPass->SetTexture(UAVTarget->GetUAVPointer());

		rootSignature = RootSignature::Resolve("StandardFullStageAccess");
	}

	RenderPipelineGraph::~RenderPipelineGraph()
	{
	}

	void RenderPipelineGraph::SetRenderQueue(std::shared_ptr<DrawableObject::Drawable> inputObject)
	{
		drawableObjects.emplace_back(inputObject);
	}

	void RenderPipelineGraph::Bind() noxnd
	{
		Graphics::GetInstance().BindShaderDescriptorHeaps();
		rootSignature->Bind();

		Graphics::GetInstance().GetCamera()->Bind();

		HDRTarget->BindTarget(Graphics::GetInstance().GetMainDS());
		FLOAT clearColor[] = ClearMainRTColor;
		Graphics::GetInstance().GetGraphicsCommandList()->ClearRenderTargetView(HDRTarget->GetRTVCPUDescriptorHandle(), clearColor, 0, nullptr);

		for (auto& drawableObject : drawableObjects)
		{
			drawableObject->Bind();
		}

		HDRTarget->Bind();
		UAVTarget->GetUAVPointer()->Bind();
		uavPass->Bind();

		Graphics::GetInstance().GetCommandList()->CopyResource(Graphics::GetInstance().GetCurrentBackBuffer()->GetRenderTargetBuffer(), UAVTarget->GetUAVPointer()->GetUnorderedAccessBuffer());

		drawableObjects.clear();

		ShaderManager::GetInstance().ResetAllShaderDirtyState();
	}
}
