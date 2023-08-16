#include "DepthStencil.h"

#include "RenderTarget.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	DepthStencil::DepthStencil(Graphics& gfx, UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DescriptorAllocation> inputDescriptorAllocation, UINT inputDepth)
		:
		DepthStencil{ gfx.GetDecive(), inputWidth, inputHeight, inputDescriptorAllocation, inputDepth }
	{
		
	}

	DepthStencil::DepthStencil(wrl::ComPtr<ID3D12Device10> device, UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DescriptorAllocation> inputDescriptorAllocation, UINT inputDepth)
		:
		descriptorAllocation(inputDescriptorAllocation),
		cpuHandle(inputDescriptorAllocation->GetCPUDescriptorHandle(inputDepth)),
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

		Resize(device, inputWidth, inputHeight, inputDepth);
	}

	void DepthStencil::Bind(DEGraphics::Graphics& gfx) noxnd
	{

	}

	void DepthStencil::BindTarget(Graphics& gfx) noxnd
	{
		gfx.GetGraphicsCommandList()->OMSetRenderTargets(0, nullptr, FALSE, &cpuHandle);
	}

	void DepthStencil::BindTarget(DEGraphics::Graphics& gfx, std::shared_ptr<BindableTarget> renderTarget) noxnd
	{
		assert(dynamic_cast<RenderTarget*>(renderTarget.get()) != nullptr);

		D3D12_CPU_DESCRIPTOR_HANDLE RTDescHeapHandle = static_cast<RenderTarget*>(renderTarget.get())->GetDescriptorHandle();
		gfx.GetGraphicsCommandList()->OMSetRenderTargets(1, &RTDescHeapHandle, FALSE, &cpuHandle);
	}

	wrl::ComPtr<ID3D12Resource> DepthStencil::GetDepthStencilBuffer() const noexcept
	{
		return depthStencilBuffer;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencil::GetDescriptorHandle() const noexcept
	{
		return cpuHandle;
	}

	void DepthStencil::ClearDepth(Graphics& gfx, FLOAT clearDepth) const noexcept
	{
		ClearDepth(gfx.GetGraphicsCommandList(), clearDepth);
	}

	void DepthStencil::ClearDepth(wrl::ComPtr<ID3D12GraphicsCommandList7> commandList, FLOAT clearDepth) const noexcept
	{
		commandList->ClearDepthStencilView(GetDescriptorHandle(), D3D12_CLEAR_FLAG_DEPTH, clearDepth, 0, 0, nullptr);
	}

	void DepthStencil::Resize(Graphics& gfx, const UINT inputWidth, const UINT inputHeight, const UINT inputDepth)
	{
		Resize(gfx.GetDecive(), inputWidth, inputHeight, inputDepth);
	}

	void DepthStencil::Resize(wrl::ComPtr<ID3D12Device10> device, const UINT inputWidth, const UINT inputHeight, const UINT inputDepth)
	{
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);
		depth = std::max(1u, inputDepth);

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&depthStencilBuffer)
		));

		device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsv,
			cpuHandle);
	}
}
