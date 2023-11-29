#include "RenderTarget.h"

#include "DepthStencil.h"
#include "..\GraphicsSource.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\ResourceStateTracker.h"
#include "..\DX\CommandList.h"

namespace DiveBomber::DEResource
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	RenderTarget::RenderTarget(wrl::ComPtr<ID3D12Resource> inputBuffer,
		std::shared_ptr<DescriptorAllocator> inputRTVDescriptorAllocator)
		:
		Resource(L"?"),
		rtvDescriptorAllocator(inputRTVDescriptorAllocator),
		rtvDescriptorAllocation(rtvDescriptorAllocator->Allocate(1u)),
		rtvCPUHandle(rtvDescriptorAllocation->GetCPUDescriptorHandle()),
		optimizedClearValue(),
		//for partial view someday
		rsvDesc()
	{
		Resize(inputBuffer);
	}

	RenderTarget::RenderTarget(std::shared_ptr<DX::DescriptorAllocator> inputRTVDescriptorAllocator,
		CD3DX12_RESOURCE_DESC inputDesc, bool updateRT)
		:
		Resource(L"?"),
		rtvDescriptorAllocator(inputRTVDescriptorAllocator),
		rtvDescriptorAllocation(rtvDescriptorAllocator->Allocate(1u)),
		rtvCPUHandle(rtvDescriptorAllocation->GetCPUDescriptorHandle()),
		rsvDesc(),

	{
		// Resize screen dependent resources.
		// Create a render target buffer.
		optimizedClearValue.Format = rsvDesc.Format;

		if (updateRT)
		{
			Resize(inputWidth, inputHeight);
		}
	}

	RenderTarget::~RenderTarget()
	{
		ResourceStateTracker::RemoveGlobalResourceState(renderTargetBuffer);
	}

	void RenderTarget::BindTarget() noxnd
	{
		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(renderTargetBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);

		Graphics::GetInstance().GetGraphicsCommandList()->OMSetRenderTargets(1, &rtvCPUHandle, FALSE, nullptr);
	}

	void RenderTarget::BindTarget(std::shared_ptr<DepthStencil> depthStencil) noxnd
	{
		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(renderTargetBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);

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

	void RenderTarget::Resize(const D3D12_RENDER_TARGET_VIEW_DESC inputDesc)
	{
		rsvDesc = inputDesc;

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		auto device = Graphics::GetInstance().GetDevice();

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_COMMON,
			&optimizedClearValue,
			IID_PPV_ARGS(&renderTargetBuffer)
		));

		ResourceStateTracker::AddGlobalResourceState(renderTargetBuffer, D3D12_RESOURCE_STATE_COMMON);

		device->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, rtvCPUHandle);
	}

	void RenderTarget::Resize(const wrl::ComPtr<ID3D12Resource> newbuffer)
	{
		//renderTargetBuffer address changed
		if (renderTargetBuffer)
		{
			ResourceStateTracker::RemoveGlobalResourceState(renderTargetBuffer);
		}

		renderTargetBuffer = newbuffer;

		ResourceStateTracker::AddGlobalResourceState(renderTargetBuffer, D3D12_RESOURCE_STATE_COMMON);

		D3D12_RESOURCE_DESC textureDesc;
		textureDesc = renderTargetBuffer->GetDesc();
		width = (UINT)textureDesc.Width;
		height = (UINT)textureDesc.Height;
		mipLevels = textureDesc.MipLevels;
		format = textureDesc.Format;

		Graphics::GetInstance().GetDevice()->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, rtvCPUHandle);
	}

	void DiveBomber::DEResource::RenderTarget::ReleaseBuffer()
	{
		ResourceStateTracker::RemoveGlobalResourceState(renderTargetBuffer);
		renderTargetBuffer.Reset();
	}
}
