#include "RenderPipelineGraph.h"

#include "..\Graphics.h"
#include "..\DX\ShaderManager.h"
#include "..\Component\Camera.h"
#include "..\BindableObject\RenderTarget.h"
#include "..\BindableObject\DepthStencil.h"
#include "..\BindableObject\RootSignature.h"

#include "..\BindableObject\RenderTargetAsShaderResourceView.h"
#include "..\BindableObject\UnorderedAccessBuffer.h"

#include "..\DrawableObject\FullScreenPlane.h"

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
		fullScreenPlane = std::make_shared<FullScreenPlane>(L"FullScreenPlane");

		HDRTarget = std::make_shared<RenderTargetAsShaderResourceView>(
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			DXGI_FORMAT_R32G32B32A32_FLOAT
			);

		UAVTarget = std::make_shared<UnorderedAccessBuffer>(
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			DXGI_FORMAT_B8G8R8A8_UNORM
		);

		fullScreenPlane->SetTexture(HDRTarget);

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

		HDRTarget->BindTarget(Graphics::GetInstance().GetMainDS());
		FLOAT clearColor[] = ClearMainRTColor;
		Graphics::GetInstance().GetGraphicsCommandList()->ClearRenderTargetView(HDRTarget->GetRTVCPUDescriptorHandle(), clearColor, 0, nullptr);

		Graphics::GetInstance().GetCamera()->Bind();
		for (auto& drawableObject : drawableObjects)
		{
			drawableObject->Bind();
		}

		Graphics::GetInstance().GetCurrentBackBuffer()->BindTarget();
		HDRTarget->Bind();
		fullScreenPlane->Bind();

		drawableObjects.clear();

		ShaderManager::GetInstance().ResetAllShaderDirtyState();
	}
}
