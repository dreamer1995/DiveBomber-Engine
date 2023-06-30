#include "RenderTarget.h"

#include "DepthStencil.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	RenderTarget::RenderTarget(DEGraphics::Graphics& gfx, wrl::ComPtr<ID3D12Resource> inputBuffer,
		std::shared_ptr<DX::DescriptorHeap> inputDescHeap, UINT inputDepth)
		:
		RenderTarget{ gfx.GetDecive(), inputBuffer, inputDescHeap, inputDepth }
	{
	}

	RenderTarget::RenderTarget(wrl::ComPtr<ID3D12Device2> device, wrl::ComPtr<ID3D12Resource> inputBuffer,
		std::shared_ptr<DX::DescriptorHeap> inputDescHeap, UINT inputDepth)
	{
		renderTargetBuffer = inputBuffer;

		D3D12_RESOURCE_DESC textureDesc;
		textureDesc = renderTargetBuffer->GetDesc();
		width = (UINT)textureDesc.Width;
		height = (UINT)textureDesc.Height;
		renderTargetDescHeap = inputDescHeap;
		depth = std::max(0u, inputDepth);
		mipLevels = textureDesc.MipLevels;

		UINT rtvDescriptorSize = 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(renderTargetDescHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
		descriptorHandle = rtvHandle;
		if (depth > 0)
		{
			rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * depth;
			descriptorHandle.Offset(rtvDescriptorSize);
		}

		device->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, descriptorHandle);
	}

	void RenderTarget::Bind(DEGraphics::Graphics& gfx) noxnd
	{

	}

	RenderTarget::RenderTarget(Graphics& gfx, UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DX::DescriptorHeap> inputDescHeap, DXGI_FORMAT inputFormat, UINT inputDepth, UINT inputMipLevels)
		:
		descriptorHandle(inputDescHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart())
	{
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);
		renderTargetDescHeap = inputDescHeap;
		depth = std::max(1u, inputDepth);
		mipLevels = inputMipLevels;

		UINT rtvDescriptorSize = 0;
		if (depth > 0)
		{
			rtvDescriptorSize = gfx.GetDecive()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * depth;
			descriptorHandle.Offset(rtvDescriptorSize);
		}
		
		// Resize screen dependent resources.
		// Create a render target buffer.
		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = format;

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(inputFormat, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		GFX_THROW_INFO(gfx.GetDecive()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&optimizedClearValue,
			IID_PPV_ARGS(&renderTargetBuffer)
		));

		// Update the depth-stencil view.
		D3D12_RENDER_TARGET_VIEW_DESC rsv = {};
		rsv.Format = format;
		rsv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rsv.Texture2D.MipSlice = mipLevels;

		gfx.GetDecive()->CreateRenderTargetView(renderTargetBuffer.Get(), &rsv,
			descriptorHandle);
	}

	void RenderTarget::BindTarget(DEGraphics::Graphics& gfx) noxnd
	{
		gfx.GetCommandList()->OMSetRenderTargets(1, &descriptorHandle, FALSE, nullptr);
	}

	void RenderTarget::BindTarget(DEGraphics::Graphics& gfx, std::shared_ptr<BindableTarget> depthStencil) noxnd
	{
		assert(dynamic_cast<DepthStencil*>(depthStencil.get()) != nullptr);

		CD3DX12_CPU_DESCRIPTOR_HANDLE depthDescHeapHandle = static_cast<DepthStencil*>(depthStencil.get())->GetDescriptorHandle();
		gfx.GetCommandList()->OMSetRenderTargets(1, &descriptorHandle, FALSE, &depthDescHeapHandle);
	}

	wrl::ComPtr<ID3D12Resource> RenderTarget::GetRenderTargetBuffer() const noexcept
	{
		return renderTargetBuffer;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetDescriptorHandle() const noexcept
	{
		return descriptorHandle;
	}
}
