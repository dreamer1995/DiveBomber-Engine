#include "RenderTargetAsShaderResourceView.h"

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

	RenderTargetAsShaderResourceView::RenderTargetAsShaderResourceView(UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DX::DescriptorAllocator> inputRTVDescriptorAllocator,
		std::shared_ptr<DX::DescriptorAllocator> inputSRVDescriptorAllocator,
		DXGI_FORMAT inputFormat, UINT inputMipLevels)
		:
		RenderTarget(inputWidth, inputHeight, inputRTVDescriptorAllocator, inputFormat, inputMipLevels),
		srvDescriptorAllocator(inputSRVDescriptorAllocator),
		srvDescriptorAllocation(srvDescriptorAllocator->Allocate(1u)),
		srvCPUHandle(srvDescriptorAllocation->GetCPUDescriptorHandle()),
		srv()
	{

	}

	RenderTargetAsShaderResourceView::~RenderTargetAsShaderResourceView()
	{
	}

	void RenderTargetAsShaderResourceView::Bind() noxnd
	{
		ResourceStateTracker::AddGlobalResourceState(renderTargetBuffer, D3D12_RESOURCE_STATE_COMMON);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetAsShaderResourceView::GetSRVCPUDescriptorHandle() const noexcept
	{
		return srvCPUHandle;
	}

	UINT RenderTargetAsShaderResourceView::GetSRVDescriptorHeapOffset() const noexcept
	{
		return srvDescriptorAllocation->GetBaseOffset();
	}

	void RenderTargetAsShaderResourceView::Resize(const UINT inputWidth, const UINT inputHeight)
	{
		RenderTarget::Resize(inputWidth, inputHeight);

		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_NONE);

		auto device = Graphics::GetInstance().GetDevice();

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_COMMON,
			&optimizedClearValue,
			IID_PPV_ARGS(&renderTargetBuffer)
		));

		device->CreateShaderResourceView(renderTargetBuffer.Get(), &srv, srvCPUHandle);
	}
}
