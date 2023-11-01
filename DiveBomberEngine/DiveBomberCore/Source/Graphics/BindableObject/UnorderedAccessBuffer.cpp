#include "UnorderedAccessBuffer.h"

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

	UnorderedAccessBuffer::UnorderedAccessBuffer(UINT inputWidth, UINT inputHeight,
		std::shared_ptr<DX::DescriptorAllocator> inputDescriptorAllocator,
		DXGI_FORMAT inputFormat, UINT inputMipLevels)
		:
		width(inputWidth),
		height(inputHeight),
		descriptorAllocator(inputDescriptorAllocator),
		uavDescriptorAllocation(descriptorAllocator->Allocate(1u)),
		uavCPUHandle(uavDescriptorAllocation->GetCPUDescriptorHandle()),
		uav(),
		srvDescriptorAllocation(descriptorAllocator->Allocate(1u)),
		srvCPUHandle(srvDescriptorAllocation->GetCPUDescriptorHandle()),
		srv(),
		mipLevels(inputMipLevels),
		format(inputFormat)
	{
		srv.Format = format;
		srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MipLevels = mipLevels;
		srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		uav.Format = format;
		uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uav.Texture2D.MipSlice = mipLevels;

		Resize(inputWidth, inputHeight);
	}

	UnorderedAccessBuffer::~UnorderedAccessBuffer()
	{
		ResourceStateTracker::RemoveGlobalResourceState(unorderedAccessBuffer);
	}

	void UnorderedAccessBuffer::Bind() noxnd
	{
	}

	void UnorderedAccessBuffer::BindAsUAV() noxnd
	{
		ResourceStateTracker::AddGlobalResourceState(unorderedAccessBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	void UnorderedAccessBuffer::BindAsSRV() noxnd
	{
		ResourceStateTracker::AddGlobalResourceState(unorderedAccessBuffer, D3D12_RESOURCE_STATE_COMMON);
	}

	wrl::ComPtr<ID3D12Resource> UnorderedAccessBuffer::GetUnorderedAccessBuffer() const noexcept
	{
		return unorderedAccessBuffer;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccessBuffer::GetUAVCPUDescriptorHandle() const noexcept
	{
		return uavCPUHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccessBuffer::GetSRVCPUDescriptorHandle() const noexcept
	{
		return srvCPUHandle;
	}

	void UnorderedAccessBuffer::Resize(const UINT inputWidth, const UINT inputHeight)
	{
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		auto device = Graphics::GetInstance().GetDevice();

		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&unorderedAccessBuffer)
		));

		device->CreateUnorderedAccessView(unorderedAccessBuffer.Get(), nullptr, &uav, uavCPUHandle);

		device->CreateShaderResourceView(unorderedAccessBuffer.Get(), &srv, srvCPUHandle);
	}
}
