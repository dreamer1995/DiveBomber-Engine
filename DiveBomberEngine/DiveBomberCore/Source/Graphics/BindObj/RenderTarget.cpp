#include "RenderTarget.h"

#include "DepthStencil.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\DX\DescriptorHeap.h"

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
		:
		renderTargetBuffer(inputBuffer),
		renderTargetDescHeap(inputDescHeap),
		depth(std::max(0u, inputDepth)),
		optimizedClearValue(),
		rsv()
	{
		D3D12_RESOURCE_DESC textureDesc;
		textureDesc = renderTargetBuffer->GetDesc();
		width = (UINT)textureDesc.Width;
		height = (UINT)textureDesc.Height;
		mipLevels = textureDesc.MipLevels;
		format = textureDesc.Format;

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
		RenderTarget{ gfx.GetDecive(), inputWidth, inputHeight, inputDescHeap, inputFormat, inputDepth, inputMipLevels }
	{

	}

	RenderTarget::RenderTarget(wrl::ComPtr<ID3D12Device2> device, UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DX::DescriptorHeap> inputDescHeap, DXGI_FORMAT inputFormat, UINT inputDepth, UINT inputMipLevels)
		:
		renderTargetDescHeap(inputDescHeap),
		mipLevels(inputMipLevels),
		format(inputFormat)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(renderTargetDescHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
		descriptorHandle = rtvHandle;

		// Resize screen dependent resources.
		// Create a render target buffer.
		optimizedClearValue.Format = format;

		// Update the depth-stencil view.
		rsv.Format = format;
		rsv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rsv.Texture2D.MipSlice = mipLevels;

		Resize(device, inputWidth, inputHeight, inputDepth);
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

	void RenderTarget::Resize(Graphics& gfx, const UINT inputWidth, const UINT inputHeight, const UINT inputDepth)
	{
		Resize(gfx.GetDecive(), inputWidth, inputHeight, inputDepth);
	}
	void RenderTarget::Resize(wrl::ComPtr<ID3D12Device2> device, const UINT inputWidth, const UINT inputHeight, const UINT inputDepth)
	{
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);
		depth = std::max(1u, inputDepth);

		UINT rtvDescriptorSize = 0;
		if (depth > 0)
		{
			rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * depth;
			descriptorHandle.Offset(rtvDescriptorSize);
		}

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&optimizedClearValue,
			IID_PPV_ARGS(&renderTargetBuffer)
		));

		device->CreateRenderTargetView(renderTargetBuffer.Get(), &rsv,
			descriptorHandle);
	}
}
