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
	IndexBuffer::IndexBuffer(Graphics& gfx, std::string tag, const std::vector<unsigned short>& indices)
		:
		tag(tag),
		count((UINT)indices.size())
	{
		size_t bufferSize = UINT(count * sizeof(unsigned short));

		HRESULT hr;

		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resDes = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);

		void* bufferData = (void*)indices.data();

		// Create a committed resource for the GPU resource in a default heap.
		GFX_THROW_INFO(gfx.GetDecive()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&indexBuffer)));

		// Create an committed resource for the upload.
		if (indices.data())
		{
			wrl::ComPtr<ID3D12Resource> intermediateIndexBuffer;

			auto heapProp1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto resDes1 = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

			GFX_THROW_INFO(gfx.GetDecive()->CreateCommittedResource(
				&heapProp1,
				D3D12_HEAP_FLAG_NONE,
				&resDes1,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&intermediateIndexBuffer)));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = bufferData;
			subresourceData.RowPitch = bufferSize;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			UpdateSubresources(gfx.GetCommandList(D3D12_COMMAND_LIST_TYPE_COPY),
				indexBuffer.Get(), intermediateIndexBuffer.Get(),
				0, 0, 1, &subresourceData);
		}

		// Create index buffer view.
		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		indexBufferView.SizeInBytes = bufferSize;

		//D3D11_BUFFER_DESC ibd = {};
		//ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		//ibd.Usage = D3D11_USAGE_DEFAULT;
		//ibd.CPUAccessFlags = 0u;
		//ibd.MiscFlags = 0u;
		//ibd.ByteWidth = UINT(count * sizeof(unsigned short));
		//ibd.StructureByteStride = sizeof(unsigned short);
		//D3D11_SUBRESOURCE_DATA isd = {};
		//isd.pSysMem = indices.data();
		//GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &pIndexBuffer));
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
