#include "RenderTarget.h"

#include "DepthStencil.h"
#include "..\GraphicsSource.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\ResourceStateTracker.h"
#include "..\DX\CommandList.h"

namespace DiveBomber::GraphicResource
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
		rtvDesc(),
		resourceDesc(),
		renderTargetBuffer(inputBuffer),
		selfManagedBuffer(false)
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
		rtvDesc(),
		resourceDesc(inputDesc),
		selfManagedBuffer(true)
	{
		// Resize screen dependent resources.
		// Create a render target buffer.
		optimizedClearValue.Format = resourceDesc.Format;

		if (updateRT)
		{
			Resize(resourceDesc);
		}
	}

	RenderTarget::~RenderTarget()
	{
		if (selfManagedBuffer)
		{
			ResourceStateTracker::RemoveGlobalResourceState(renderTargetBuffer);
		}
	}

	void RenderTarget::BindTarget() noxnd
	{
		TransitStateToRT();

		Graphics::GetInstance().GetGraphicsCommandList()->OMSetRenderTargets(1, &rtvCPUHandle, FALSE, nullptr);
	}

	void RenderTarget::BindTarget(std::shared_ptr<DepthStencil> depthStencil) noxnd
	{
		TransitStateToRT();

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

	void RenderTarget::Resize(const CD3DX12_RESOURCE_DESC inputDesc)
	{
		if (!selfManagedBuffer)
		{
			throw std::exception{ "Try to overwrite external buffer!" };
		}

		resourceDesc = inputDesc;
		optimizedClearValue.Format = resourceDesc.Format;

		rtvDesc.Format = resourceDesc.Format;

		if (resourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
		{
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		}
		else if (resourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
		{
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
		}

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		auto device = Graphics::GetInstance().GetDevice();

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optimizedClearValue,
			IID_PPV_ARGS(&renderTargetBuffer)
		));

		ResourceStateTracker::AddGlobalResourceState(renderTargetBuffer, D3D12_RESOURCE_STATE_COMMON);

		device->CreateRenderTargetView(renderTargetBuffer.Get(), &rtvDesc, rtvCPUHandle);
	}

	void DiveBomber::GraphicResource::RenderTarget::Resize(UINT width, UINT height)
	{
		// Don't allow 0 size swap chain back buffers.
		width = std::max(1u, width);
		height = std::max(1u, height);

		resourceDesc.Width = width;
		resourceDesc.Height = height;
		Resize(resourceDesc);
	}

	void RenderTarget::Resize(const wrl::ComPtr<ID3D12Resource> newbuffer)
	{
		if (selfManagedBuffer)
		{
			throw std::exception{ "Try to overwrite internal buffer!" };
		}

		//renderTargetBuffer address changed
		if (renderTargetBuffer != newbuffer)
		{
			ResourceStateTracker::RemoveGlobalResourceState(renderTargetBuffer);
		}

		renderTargetBuffer = newbuffer;
		resourceDesc = CD3DX12_RESOURCE_DESC(newbuffer->GetDesc());

		ResourceStateTracker::AddGlobalResourceState(renderTargetBuffer, D3D12_RESOURCE_STATE_COMMON);

		Graphics::GetInstance().GetDevice()->CreateRenderTargetView(renderTargetBuffer.Get(), nullptr, rtvCPUHandle);
	}

	void RenderTarget::ReleaseBuffer()
	{
		ResourceStateTracker::RemoveGlobalResourceState(renderTargetBuffer);
		renderTargetBuffer.Reset();
	}

	void RenderTarget::TransitStateToRT() noxnd
	{
		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(renderTargetBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	}

	void RenderTarget::Clear(FLOAT clearColor[]) noxnd
	{
		Graphics::GetInstance().GetGraphicsCommandList()->ClearRenderTargetView(rtvCPUHandle, clearColor, 0, nullptr);
	}
}
