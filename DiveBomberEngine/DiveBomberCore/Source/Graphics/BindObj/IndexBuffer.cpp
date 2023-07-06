#include "IndexBuffer.h"
#include "BindableCodex.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices)
		:
		IndexBuffer(gfx, "?", indices)
	{}
	IndexBuffer::IndexBuffer(Graphics& gfx, std::string inputTag, const std::vector<unsigned short>& indices)
	{
		tag = inputTag;
		count = (UINT)indices.size();

		size_t bufferSize = count * sizeof(unsigned short);

		HRESULT hr;

		const CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_DEFAULT };
		const CD3DX12_RESOURCE_DESC resDes = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);

		// Create a committed resource for the GPU resource in a default heap.
		GFX_THROW_INFO(gfx.GetDecive()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&indexBuffer)));

		// Create an committed resource for the upload.
		if (indices.data())
		{
			const CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
			const CD3DX12_RESOURCE_DESC resDes = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

			GFX_THROW_INFO(gfx.GetDecive()->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDes,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&indexUploadBuffer)));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = indices.data();
			subresourceData.RowPitch = bufferSize;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			UpdateSubresources(gfx.GetCommandList(D3D12_COMMAND_LIST_TYPE_COPY).Get(),
				indexBuffer.Get(), indexUploadBuffer.Get(),
				0, 0, 1, &subresourceData);
		}

		const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			indexBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		gfx.GetCommandList()->ResourceBarrier(1, &barrier);

		// Create index buffer view.
		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		indexBufferView.SizeInBytes = (UINT)bufferSize;
	}

	void IndexBuffer::Bind(Graphics& gfx) noxnd
	{
		GFX_THROW_INFO_ONLY(gfx.GetCommandList()->IASetIndexBuffer(&indexBufferView));
	}

	UINT IndexBuffer::GetCount() const noexcept
	{
		return count;
	}
	std::shared_ptr<IndexBuffer> IndexBuffer::Resolve(Graphics& gfx, const std::string& tag,
		const std::vector<unsigned short>& indices)
	{
		assert(tag != "?");
		return Codex::Resolve<IndexBuffer>(gfx, tag, indices);
	}
	std::string IndexBuffer::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(IndexBuffer).name() + "#"s + tag;
	}
	std::string IndexBuffer::GetUID() const noexcept
	{
		return GenerateUID_(tag);
	}
}
