#include "RenderTarget.h"

#include "DepthStencil.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\ResourceStateTracker.h"

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	RenderTarget::RenderTarget(wrl::ComPtr<ID3D12Resource> inputBuffer,
		std::shared_ptr<DescriptorAllocator> inputRTVDescriptorAllocator)
		:
		rtvDescriptorAllocator(inputRTVDescriptorAllocator),
		rtvDescriptorAllocation(rtvDescriptorAllocator->Allocate(1u)),
		rtvCPUHandle(rtvDescriptorAllocation->GetCPUDescriptorHandle()),
		optimizedClearValue(),
		//for partial view someday
		rsv()
	{
		Resize(inputBuffer);
	}

	RenderTarget::RenderTarget(UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DX::DescriptorAllocator> inputRTVDescriptorAllocator,
		DXGI_FORMAT inputFormat, UINT inputMipLevels, bool updateRT)
		: 
		rtvDescriptorAllocator(inputRTVDescriptorAllocator),
		rtvDescriptorAllocation(rtvDescriptorAllocator->Allocate(1u)),
		rtvCPUHandle(rtvDescriptorAllocation->GetCPUDescriptorHandle()),
		mipLevels(inputMipLevels),
		format(inputFormat),
		rsv()
	{
		// Resize screen dependent resources.
		// Create a render target buffer.
		optimizedClearValue.Format = format;

		if (updateRT)
		{
			Resize(inputWidth, inputHeight);
		}
	}

	RenderTarget::~RenderTarget()
	{
		ResourceStateTracker::RemoveGlobalResourceState(renderTargetBuffer);
	}

	void RenderTarget::Bind() noxnd
	{

	}

	void RenderTarget::BindTarget() noxnd
	{
		ResourceStateTracker::AddGlobalResourceState(renderTargetBuffer, D3D12_RESOURCE_STATE_COMMON);

		Graphics::GetInstance().GetGraphicsCommandList()->OMSetRenderTargets(1, &rtvCPUHandle, FALSE, nullptr);
	}

	void RenderTarget::BindTarget(std::shared_ptr<BindableTarget> depthStencil) noxnd
	{
		ResourceStateTracker::AddGlobalResourceState(renderTargetBuffer, D3D12_RESOURCE_STATE_COMMON);

		D3D12_CPU_DESCRIPTOR_HANDLE depthDescHeapHandle = std::dynamic_pointer_cast<DepthStencil>(depthStencil)->GetDSVCPUDescriptorHandle();
		Graphics::GetInstance().GetGraphicsCommandList()->OMSetRenderTargets(1, &rtvCPUHandle, FALSE, &depthDescHeapHandle);
	}

	wrl::ComPtr<ID3D12Resource> RenderTarget::GetRenderTargetBuffer() const noexcept
	{
		return renderTargetBuffer;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetRTVCPUDescriptorHandle() const noexcept
	{
		return rtvCPUHandle;
	}

	void RenderTarget::Resize(const UINT inputWidth, const UINT inputHeight)
	{
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		auto device = Graphics::GetInstance().GetDevice();

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&optimizedClearValue,
			IID_PPV_ARGS(&renderTargetBuffer)
		));

		device->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, rtvCPUHandle);
	}

	void RenderTarget::Resize(wrl::ComPtr<ID3D12Resource> newbuffer)
	{
		renderTargetBuffer = newbuffer;

		D3D12_RESOURCE_DESC textureDesc;
		textureDesc = renderTargetBuffer->GetDesc();
		width = (UINT)textureDesc.Width;
		height = (UINT)textureDesc.Height;
		mipLevels = textureDesc.MipLevels;
		format = textureDesc.Format;

		Graphics::GetInstance().GetDevice()->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, rtvCPUHandle);
	}

	void DiveBomber::BindableObject::RenderTarget::ReleaseBuffer()
	{
		ResourceStateTracker::RemoveGlobalResourceState(renderTargetBuffer);
		renderTargetBuffer.Reset();
	}
}
