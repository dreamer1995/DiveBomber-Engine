#include "RenderTarget.h"

#include "DepthStencil.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\ResourceStateTracker.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	RenderTarget::RenderTarget(DEGraphics::Graphics& gfx, wrl::ComPtr<ID3D12Resource> inputBuffer,
		std::shared_ptr<DescriptorAllocation> inputDescriptorAllocation, UINT inputDepth)
		:
		RenderTarget{ gfx.GetDecive(), inputBuffer, inputDescriptorAllocation, inputDepth }
	{
	}

	RenderTarget::RenderTarget(wrl::ComPtr<ID3D12Device2> device, wrl::ComPtr<ID3D12Resource> inputBuffer,
		std::shared_ptr<DescriptorAllocation> inputDescriptorAllocation, UINT inputDepth)
		:
		renderTargetBuffer(inputBuffer),
		descriptorAllocation(inputDescriptorAllocation),
		cpuHandle(inputDescriptorAllocation->GetCPUDescriptorHandle(inputDepth)),
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

		device->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, cpuHandle);
		ResourceStateTracker::AddGlobalResourceState(renderTargetBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	RenderTarget::RenderTarget(Graphics& gfx, UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DescriptorAllocation> inputDescriptorAllocation, DXGI_FORMAT inputFormat, UINT inputDepth, UINT inputMipLevels)
		:
		RenderTarget{ gfx.GetDecive(), inputWidth, inputHeight, inputDescriptorAllocation, inputFormat, inputDepth, inputMipLevels }
	{
	}

	RenderTarget::RenderTarget(wrl::ComPtr<ID3D12Device2> device, UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DescriptorAllocation> inputDescriptorAllocation, DXGI_FORMAT inputFormat, UINT inputDepth, UINT inputMipLevels)
		:
		descriptorAllocation(inputDescriptorAllocation),
		cpuHandle(inputDescriptorAllocation->GetCPUDescriptorHandle(inputDepth)),
		mipLevels(inputMipLevels),
		format(inputFormat)
	{
		// Resize screen dependent resources.
		// Create a render target buffer.
		optimizedClearValue.Format = format;

		// Update the depth-stencil view.
		rsv.Format = format;
		rsv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rsv.Texture2D.MipSlice = mipLevels;

		Resize(device, inputWidth, inputHeight, inputDepth);
	}

	RenderTarget::~RenderTarget()
	{
		ResourceStateTracker::RemoveGlobalResourceState(renderTargetBuffer);
	}

	void RenderTarget::Bind(DEGraphics::Graphics& gfx) noxnd
	{

	}

	void RenderTarget::BindTarget(DEGraphics::Graphics& gfx) noxnd
	{
		gfx.GetCommandList()->OMSetRenderTargets(1, &cpuHandle, FALSE, nullptr);
	}

	void RenderTarget::BindTarget(DEGraphics::Graphics& gfx, std::shared_ptr<BindableTarget> depthStencil) noxnd
	{
		assert(dynamic_cast<DepthStencil*>(depthStencil.get()) != nullptr);

		D3D12_CPU_DESCRIPTOR_HANDLE depthDescHeapHandle = static_cast<DepthStencil*>(depthStencil.get())->GetDescriptorHandle();
		gfx.GetCommandList()->OMSetRenderTargets(1, &cpuHandle, FALSE, &depthDescHeapHandle);
	}

	wrl::ComPtr<ID3D12Resource> RenderTarget::GetRenderTargetBuffer() const noexcept
	{
		return renderTargetBuffer;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetDescriptorHandle() const noexcept
	{
		return cpuHandle;
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

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		if (renderTargetBuffer)
		{
			ResourceStateTracker::RemoveGlobalResourceState(renderTargetBuffer);
		}

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&optimizedClearValue,
			IID_PPV_ARGS(&renderTargetBuffer)
		));

		device->CreateRenderTargetView(renderTargetBuffer.Get(), &rsv,
			cpuHandle);
		ResourceStateTracker::AddGlobalResourceState(renderTargetBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
}
