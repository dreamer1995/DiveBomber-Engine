#pragma once
#include "Bindable.h"
#include "..\Graphics.h"
#include "BindableCodex.h"
#include "..\..\Exception\GraphicsExceptionEX.h"

namespace DiveBomber::BindObj
{
	template<typename C>
	class ConstantBuffer final: public Bindable
	{
	public:
		ConstantBuffer(DEGraphics::Graphics& gfx, const std::string& inputTag, const C& constantData, UINT inputSlot)
		{
			tag = inputTag;
			slot = inputSlot;
			bufferSize = UINT(sizeof(constantData));

			if (bufferSize > 0)
			{
				InitializeConstantBufferSize(gfx);
				Update(gfx, constantData);
			}
			else
			{
				std::string outPutString = "Constant in " + tag + " has no data!";
				throw std::exception(outPutString.c_str());
			}

		}

		ConstantBuffer(DEGraphics::Graphics& gfx, const std::string& inputTag, UINT inputSlot)
		{
			tag = inputTag;
			slot = inputSlot;

			InitializeConstantBufferSize(gfx);
		}

		void InitializeConstantBufferSize(DEGraphics::Graphics& gfx)
		{
			HRESULT hr;

			size_t initBufferSize = bufferSize != 0 ? bufferSize : 1u;
			const CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_DEFAULT };
			const CD3DX12_RESOURCE_DESC resDes = CD3DX12_RESOURCE_DESC::Buffer(initBufferSize, D3D12_RESOURCE_FLAG_NONE);

			// Create a committed resource for the GPU resource in a default heap.
			GFX_THROW_INFO_NAMESPACE(gfx.GetDecive()->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDes,
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(&constantBuffer)));

			{
				const CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
				const CD3DX12_RESOURCE_DESC resDes = CD3DX12_RESOURCE_DESC::Buffer(initBufferSize);

				GFX_THROW_INFO_NAMESPACE(gfx.GetDecive()->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDes,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&constantUploadBuffer)));
			}
		}

		void Update(DEGraphics::Graphics& gfx, const C& constantData)
		{
			size_t newBufferSize = UINT(sizeof(constantData));

			// Create an committed resource for the upload.
			if (newBufferSize > 0)
			{
				if (bufferSize != newBufferSize)
				{
					bufferSize = newBufferSize;
					InitializeConstantBufferSize(gfx);
				}

				D3D12_SUBRESOURCE_DATA subresourceData = {};
				subresourceData.pData = &constantData;
				subresourceData.RowPitch = bufferSize;
				subresourceData.SlicePitch = subresourceData.RowPitch;

				CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					constantBuffer.Get(),
					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
				gfx.GetCommandList()->ResourceBarrier(1, &barrier);

				UpdateSubresources(gfx.GetCommandList().Get(),
					constantBuffer.Get(), constantUploadBuffer.Get(),
					0, 0, 1, &subresourceData);

				barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					constantBuffer.Get(),
					D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
				gfx.GetCommandList()->ResourceBarrier(1, &barrier);
			}
			else
			{
				std::string outPutString = "Constant in " + tag + " has no data!";
				throw std::exception(outPutString.c_str());
			}
		}

		void Bind(DEGraphics::Graphics& gfx) noxnd override
		{
			gfx.GetCommandList()->SetGraphicsRootConstantBufferView(slot, constantBuffer->GetGPUVirtualAddress());
		}

		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetConstantBuffer() noexcept
		{
			return constantBuffer;
		}

		[[nodiscard]] static std::shared_ptr<ConstantBuffer> Resolve(DEGraphics::Graphics& gfx, const std::string& tag,
			const C& constantData, const UINT slot)
		{
			return Codex::Resolve<ConstantBuffer>(gfx, tag, constantData, slot);
		}

		[[nodiscard]] static std::string GenerateUID(const std::string& tag, const C&, const UINT slot)
		{
			return GenerateUID(tag, slot);
		}

		[[nodiscard]] static std::string GenerateUID(const std::string& tag, const UINT slot)
		{
			using namespace std::string_literals;
			return typeid(ConstantBuffer).name() + "#"s + tag + "#"s + std::to_string(slot);
		}

		[[nodiscard]] std::string GetUID() const noexcept override
		{
			return GenerateUID(tag, slot);
		}

	private:
		wrl::ComPtr<ID3D12Resource> constantBuffer;
		wrl::ComPtr<ID3D12Resource> constantUploadBuffer;
		std::string tag;
		UINT slot;
		size_t bufferSize = 0u;
	};
}