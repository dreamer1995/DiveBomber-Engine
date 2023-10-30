#include "DepthStencil.h"

#include "RenderTarget.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	DepthStencil::DepthStencil(UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DescriptorAllocator> inputDescriptorAllocator)
		:
		descriptorAllocator(inputDescriptorAllocator),
		descriptorAllocation(descriptorAllocator->Allocate(1u)),
		dsvCPUHandle(descriptorAllocation->GetCPUDescriptorHandle()),
		optimizedClearValue(),
		dsv()
	{
		// Resize screen dependent resources.
		// Create a depth buffer.
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		// Update the depth-stencil view.
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0u;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		Resize(inputWidth, inputHeight);
	}

	void DepthStencil::Bind() noxnd
	{

	}

	void DepthStencil::BindTarget() noxnd
	{
		Graphics::GetInstance().GetGraphicsCommandList()->OMSetRenderTargets(0, nullptr, FALSE, &dsvCPUHandle);
	}

	void DepthStencil::BindTarget(std::shared_ptr<BindableTarget> renderTarget) noxnd
	{
		D3D12_CPU_DESCRIPTOR_HANDLE RTDescHeapHandle = std::dynamic_pointer_cast<RenderTarget>(renderTarget)->GetRTVCPUDescriptorHandle();
		Graphics::GetInstance().GetGraphicsCommandList()->OMSetRenderTargets(1, &RTDescHeapHandle, FALSE, &dsvCPUHandle);
	}

	wrl::ComPtr<ID3D12Resource> DepthStencil::GetDepthStencilBuffer() const noexcept
	{
		return depthStencilBuffer;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencil::GetDSVCPUDescriptorHandle() const noexcept
	{
		return dsvCPUHandle;
	}

	void DepthStencil::ClearDepth(FLOAT clearDepth) const noexcept
	{
		ClearDepth(Graphics::GetInstance().GetGraphicsCommandList(), clearDepth);
	}

	void DepthStencil::ClearDepth(wrl::ComPtr<ID3D12GraphicsCommandList7> commandList, FLOAT clearDepth) const noexcept
	{
		commandList->ClearDepthStencilView(GetDSVCPUDescriptorHandle(), D3D12_CLEAR_FLAG_DEPTH, clearDepth, 0, 0, nullptr);
	}

	void DepthStencil::Resize(const UINT inputWidth, const UINT inputHeight)
	{
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		auto device = Graphics::GetInstance().GetDevice();

		GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&depthStencilBuffer)
		));

		device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsv,
			dsvCPUHandle);
	}
}
