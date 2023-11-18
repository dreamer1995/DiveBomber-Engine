#include "IndexBuffer.h"

#include "..\..\GraphicsSource.h"
#include "..\..\DX\CommandList.h"
#include "..\..\DX\ResourceStateTracker.h"

namespace DiveBomber::DEResource
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;

	IndexBuffer::IndexBuffer(const std::vector<unsigned short>& indices)
		:
		IndexBuffer(L"?", indices)
	{}
	IndexBuffer::IndexBuffer(std::wstring inputName, const std::vector<unsigned short>& indices)
		:
		Resource(inputName),
		count((UINT)indices.size())
	{
		size_t bufferSize = count * sizeof(unsigned short);

		HRESULT hr;

		const CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_DEFAULT };
		const CD3DX12_RESOURCE_DESC resDes = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);

		// Create a committed resource for the GPU resource in a default heap.
		GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&indexBuffer)));

		ResourceStateTracker::AddGlobalResourceState(indexBuffer, D3D12_RESOURCE_STATE_COMMON);

		// Create an committed resource for the upload.
		if (indices.data())
		{
			wrl::ComPtr<ID3D12Resource> indexUploadBuffer;
			const CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
			const CD3DX12_RESOURCE_DESC resDes = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

			GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDes,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&indexUploadBuffer)));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = indices.data();
			subresourceData.RowPitch = bufferSize;
			subresourceData.SlicePitch = sizeof(unsigned short);

			std::shared_ptr<CommandList> copyCommandList = Graphics::GetInstance().GetCommandList(D3D12_COMMAND_LIST_TYPE_COPY);

			UpdateSubresources(copyCommandList->GetGraphicsCommandList().Get(),
				indexBuffer.Get(), indexUploadBuffer.Get(),
				0, 0, 1, &subresourceData);

			copyCommandList->TrackResource(indexUploadBuffer);
		}

		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(indexBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER, true);

		// Create index buffer view.
		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		indexBufferView.SizeInBytes = (UINT)bufferSize;
	}

	IndexBuffer::~IndexBuffer()
	{
		ResourceStateTracker::RemoveGlobalResourceState(indexBuffer);
	}

	void IndexBuffer::Bind() noxnd
	{
		GFX_THROW_INFO_ONLY(Graphics::GetInstance().GetGraphicsCommandList()->IASetIndexBuffer(&indexBufferView));
	}

	UINT IndexBuffer::GetCount() const noexcept
	{
		return count;
	}

	std::string IndexBuffer::GetUID() const noexcept
	{
		return GenerateUID(name);
	}
}
