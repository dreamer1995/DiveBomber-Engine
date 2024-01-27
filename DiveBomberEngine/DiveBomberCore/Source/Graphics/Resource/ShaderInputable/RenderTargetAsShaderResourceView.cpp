#include "RenderTargetAsShaderResourceView.h"

#include "..\..\GraphicsSource.h"
#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"
#include "..\..\DX\ResourceStateTracker.h"
#include "..\..\DX\CommandList.h"

namespace DiveBomber::DEResource
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	RenderTargetAsShaderResourceView::RenderTargetAsShaderResourceView(
		std::shared_ptr<DX::DescriptorAllocator> inputRTVDescriptorAllocator,
		std::shared_ptr<DX::DescriptorAllocator> inputSRVDescriptorAllocator,
		CD3DX12_RESOURCE_DESC inputDesc)
		:
		RenderTarget(inputRTVDescriptorAllocator, inputDesc, false),
		srvDescriptorAllocator(inputSRVDescriptorAllocator),
		srvDescriptorAllocation(srvDescriptorAllocator->Allocate(1u)),
		srvCPUHandle(srvDescriptorAllocation->GetCPUDescriptorHandle()),
		srvDesc()
	{
		Resize(resourceDesc);
	}

	RenderTargetAsShaderResourceView::~RenderTargetAsShaderResourceView()
	{
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetAsShaderResourceView::GetSRVCPUDescriptorHandle() const noexcept
	{
		return srvCPUHandle;
	}

	UINT RenderTargetAsShaderResourceView::GetSRVDescriptorHeapOffset() const noexcept
	{
		return srvDescriptorAllocation->GetBaseOffset();
	}

	void RenderTargetAsShaderResourceView::BindAsShaderResource() noxnd
	{
		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(renderTargetBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, true);
	}

	void RenderTargetAsShaderResourceView::Resize(CD3DX12_RESOURCE_DESC inputDesc)
	{
		RenderTarget::Resize(inputDesc);

		auto device = Graphics::GetInstance().GetDevice();

		srvDesc.Format = resourceDesc.Format;

		if (resourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		}
		else if (resourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		}

		device->CreateShaderResourceView(renderTargetBuffer.Get(), nullptr, srvCPUHandle);
	}

	void DiveBomber::DEResource::RenderTargetAsShaderResourceView::Resize(UINT width, UINT height)
	{
		// Don't allow 0 size swap chain back buffers.
		width = std::max(1u, width);
		height = std::max(1u, height);

		resourceDesc.Width = width;
		resourceDesc.Height = height;
		Resize(resourceDesc);
	}
}
