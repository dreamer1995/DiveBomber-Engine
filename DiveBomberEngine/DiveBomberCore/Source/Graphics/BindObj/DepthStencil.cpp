#include "DepthStencil.h"

#include "RenderTarget.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	DepthStencil::DepthStencil(Graphics& gfx, UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DX::DescriptorHeap> inputDescHeap, UINT inputDepth)
		:
		descriptorHandle(inputDescHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart())
	{
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);
		depthStencilDescHeap = inputDescHeap;
		depth = std::max(0u, inputDepth);

		// Resize screen dependent resources.
		// Create a depth buffer.
		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		GFX_THROW_INFO(gfx.GetDecive()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&depthStencilBuffer)
		));

		// Update the depth-stencil view.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = depth;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		gfx.GetDecive()->CreateDepthStencilView(depthStencilBuffer.Get(), &dsv,
			descriptorHandle);
	}

	void DepthStencil::BindTarget(Graphics& gfx) noxnd
	{
		gfx.GetCommandList()->OMSetRenderTargets(0, nullptr, FALSE, &descriptorHandle);
	}

	void DepthStencil::BindTarget(DEGraphics::Graphics& gfx, std::shared_ptr<BindableTarget> renderTarget) noxnd
	{
		assert(dynamic_cast<RenderTarget*>(renderTarget.get()) != nullptr);

		CD3DX12_CPU_DESCRIPTOR_HANDLE RTDescHeapHandle = static_cast<RenderTarget*>(renderTarget.get())->GetDescriptorHandle();
		gfx.GetCommandList()->OMSetRenderTargets(1, &RTDescHeapHandle, FALSE, &descriptorHandle);
	}

	wrl::ComPtr<ID3D12Resource> DepthStencil::GetDepthStencilBuffer() const noexcept
	{
		return depthStencilBuffer;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DepthStencil::GetDescriptorHandle() const noexcept
	{
		return descriptorHandle;
	}
}
