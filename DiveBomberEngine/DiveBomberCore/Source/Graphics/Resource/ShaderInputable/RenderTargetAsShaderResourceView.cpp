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

	RenderTargetAsShaderResourceView::RenderTargetAsShaderResourceView(UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DX::DescriptorAllocator> inputRTVDescriptorAllocator,
		std::shared_ptr<DX::DescriptorAllocator> inputSRVDescriptorAllocator,
		DXGI_FORMAT inputFormat, UINT inputMipLevels)
		:
		RenderTarget(inputWidth, inputHeight, inputRTVDescriptorAllocator, inputFormat, inputMipLevels, false),
		srvDescriptorAllocator(inputSRVDescriptorAllocator),
		srvDescriptorAllocation(srvDescriptorAllocator->Allocate(1u)),
		srvCPUHandle(srvDescriptorAllocation->GetCPUDescriptorHandle()),
		srv()
	{
		Resize(inputWidth, inputHeight);
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

	void RenderTargetAsShaderResourceView::Resize(const UINT inputWidth, const UINT inputHeight)
	{
		RenderTarget::Resize(inputWidth, inputHeight);

		auto device = Graphics::GetInstance().GetDevice();

		device->CreateShaderResourceView(renderTargetBuffer.Get(), nullptr, srvCPUHandle);
	}
}
