#include "VertexBuffer.h"

#include "BindableCodex.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	VertexBuffer::VertexBuffer(Graphics& gfx, const VertexProcess::VertexData& vbuf)
		:
		VertexBuffer(gfx, "?", vbuf)
	{}
	VertexBuffer::VertexBuffer(Graphics& gfx, const std::string& inputTag, const VertexProcess::VertexData& vbuf)
	{
		stride = (UINT)vbuf.GetLayout().Size();
		tag = inputTag;
		layout = vbuf.GetLayout();

		size_t bufferSize = UINT(vbuf.SizeBytes());
		
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
			IID_PPV_ARGS(&vertexBuffer)));

		// Create an committed resource for the upload.
		if (vbuf.GetData())
		{
			const CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
			const CD3DX12_RESOURCE_DESC resDes = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

			GFX_THROW_INFO(gfx.GetDecive()->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDes,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&vertexUploadBuffer)));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = vbuf.GetData();
			subresourceData.RowPitch = bufferSize;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			UpdateSubresources(gfx.GetCommandList(D3D12_COMMAND_LIST_TYPE_COPY).Get(),
				vertexBuffer.Get(), vertexUploadBuffer.Get(),
				0, 0, 1, &subresourceData);
		}

		// Create the vertex buffer view.
		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = (UINT)bufferSize;
		vertexBufferView.StrideInBytes = stride;
	}

	const VertexProcess::VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return layout;
	}

	void VertexBuffer::Bind(Graphics& gfx) noxnd
	{
		GFX_THROW_INFO_ONLY(gfx.GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView));
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Resolve(Graphics& gfx, const std::string& tag,
		const VertexProcess::VertexData& vbuf)
	{
		assert(tag != "?");
		return Codex::Resolve<VertexBuffer>(gfx, tag, vbuf);
	}

	std::string VertexBuffer::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(VertexBuffer).name() + "#"s + tag;
	}

	std::string VertexBuffer::GetUID() const noexcept
	{
		return GenerateUID(tag);
	}
}