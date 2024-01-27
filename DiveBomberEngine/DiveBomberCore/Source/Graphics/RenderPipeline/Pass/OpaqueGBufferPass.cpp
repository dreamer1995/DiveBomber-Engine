#include "OpaqueGBufferPass.h"
#include "..\..\Graphics.h"
#include "..\..\..\Component\Camera\Camera.h"
#include "..\..\Resource\ShaderInputable\RenderTargetAsShaderResourceView.h"
#include "..\..\Resource\DepthStencil.h"
#include "..\..\Object\Object.h"

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DEComponent;
	using namespace DEResource;
	using namespace DEObject;
	using namespace DX;

	OpaqueGBufferPass::OpaqueGBufferPass(std::shared_ptr<RenderTarget> inputRenderTarget,
		std::shared_ptr<DepthStencil> inputDepthStencil)
		:
		RenderPass("OpaqueGBufferPass", inputRenderTarget, inputDepthStencil)
	{
		auto baseColorBufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		baseColorBuffer = std::make_shared<RenderTargetAsShaderResourceView>(
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			std::move(baseColorBufferDesc)
		);

		auto roughAOShadowSMIDBufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		roughAOShadowSMIDBuffer = std::make_shared<RenderTargetAsShaderResourceView>(
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			std::move(roughAOShadowSMIDBufferDesc)
		);

		auto normalBufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R10G10B10A2_UNORM,
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		normalBuffer = std::make_shared<RenderTargetAsShaderResourceView>(
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			std::move(normalBufferDesc)
		);

		auto customDataBufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,
			Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight(),
			1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		customDataBuffer = std::make_shared<RenderTargetAsShaderResourceView>(
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			std::move(customDataBufferDesc)
		);

		GBufferSet = 
		{
			baseColorBuffer,
			roughAOShadowSMIDBuffer,
			normalBuffer,
			customDataBuffer
		};

		targetHandles.emplace_back(renderTarget->GetRTVCPUDescriptorHandle());
		for (std::shared_ptr<RenderTarget> target : GBufferSet)
		{
			targetHandles.emplace_back(target->GetRTVCPUDescriptorHandle());
		}
	}

	void OpaqueGBufferPass::Execute() noxnd
	{
		if (Graphics::GetInstance().CheckResolutionDirty())
		{
			for (std::shared_ptr<RenderTarget> target : GBufferSet)
			{
				target->Resize(Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight());
			}
		}

		Graphics::GetInstance().GetCamera()->Bind();

		for (std::shared_ptr<RenderTarget> target : GBufferSet)
		{
			target->TransitStateToRT();

			FLOAT clearColor[] = ClearMainRTColor;
			target->Clear(clearColor);
		}

		renderTarget->TransitStateToRT();
		auto dsvHandle = depthStencil->GetDSVCPUDescriptorHandle();

		Graphics::GetInstance().GetGraphicsCommandList()->OMSetRenderTargets((UINT)targetHandles.size(), targetHandles.data(), FALSE, &dsvHandle);

		for (std::shared_ptr<DEObject::Object>& object : objects)
		{
			object->Render();
		}

		objects.clear();
	}
}
