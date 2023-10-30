#include "RenderTargetAsShaderResourceView.h"

#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	RenderTargetAsShaderResourceView::RenderTargetAsShaderResourceView(UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator,
		DXGI_FORMAT inputFormat, UINT inputMipLevels)
		:
		RenderTarget(inputWidth, inputHeight, inputDescriptorAllocator, inputFormat, inputMipLevels),
		srvDescriptorAllocation(inputDescriptorAllocator->Allocate(1u)),
		srvCPUHandle(srvDescriptorAllocation->GetCPUDescriptorHandle()),
		srv()
	{

	}

	RenderTargetAsShaderResourceView::~RenderTargetAsShaderResourceView()
	{
	}

	void RenderTargetAsShaderResourceView::Bind() noxnd
	{

	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetAsShaderResourceView::GetSRVCPUDescriptorHandle() const noexcept
	{
		return srvCPUHandle;
	}

	void RenderTargetAsShaderResourceView::Resize(const UINT inputWidth, const UINT inputHeight)
	{
		RenderTarget::Resize(inputWidth, inputHeight);

	}
}
