#pragma once
#include "Bindable.h"
#include "..\..\DiveBomberCore.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsExceptionEX.h"
#include "..\DX\CommandList.h"
#include "..\DX\ResourceStateTracker.h"
#include "..\DX\UploadBuffer.h"

namespace DiveBomber::BindableObject
{
	template<typename C>
	class ConstantBuffer: public Bindable
	{
	public:
		ConstantBuffer(DEGraphics::Graphics& gfx, const std::string& inputTag,
			const C* constantData, size_t inputDateSize, UINT inputSlot)
			:
			tag(inputTag),
			slot(inputSlot),
			bufferSize(inputDateSize)
		{
			if (bufferSize > 0)
			{
				InitializeConstantBufferSize(gfx);
				Update(gfx, constantData, bufferSize);
			}
			else
			{
				std::string outPutString = "Constant in " + tag + " has no data!";
				throw std::exception(outPutString.c_str());
			}
		}

		ConstantBuffer(DEGraphics::Graphics& gfx, const std::string& inputTag, UINT inputSlot)
			:
			tag(inputTag),
			slot(inputSlot)
		{
			InitializeConstantBufferSize(gfx);
		}

		~ConstantBuffer()
		{
			DX::ResourceStateTracker::RemoveGlobalResourceState(constantBuffer);
		}

		void InitializeConstantBufferSize(DEGraphics::Graphics& gfx)
		{
			HRESULT hr;

			size_t initBufferSize = bufferSize != 0 ? bufferSize : 1u;
			initBufferSize = Utility::AlignUp(initBufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
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

			DX::ResourceStateTracker::AddGlobalResourceState(constantBuffer, D3D12_RESOURCE_STATE_COMMON);
		}

		virtual void Update(DEGraphics::Graphics& gfx, const C* constantData, size_t dateSize)
		{
			// Create an committed resource for the upload.
			if (dateSize > 0)
			{
				if (bufferSize != dateSize)
				{
					bufferSize = dateSize;
					InitializeConstantBufferSize(gfx);
				}

				std::shared_ptr<DX::CommandList> commandList = gfx.GetCommandList();

				std::shared_ptr<DX::UploadBufferAllocation> uploadBufferAllocation =
					commandList->AllocateDynamicUploadBuffer(bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

				D3D12_SUBRESOURCE_DATA subresourceData = {};
				subresourceData.pData = constantData;
				subresourceData.RowPitch = bufferSize;
				subresourceData.SlicePitch = subresourceData.RowPitch;

				commandList->AddTransitionBarrier(constantBuffer, D3D12_RESOURCE_STATE_COPY_DEST, true);

				UpdateSubresources(commandList->GetGraphicsCommandList().Get(),
					constantBuffer.Get(), uploadBufferAllocation->resourceBuffer.Get(),
					uploadBufferAllocation->offset, 0, 1, &subresourceData);

				commandList->AddTransitionBarrier(constantBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, true);
			}
			else
			{
				std::string outPutString = "Constant in " + tag + " has no data!";
				throw std::exception(outPutString.c_str());
			}
		}

		void Bind(DEGraphics::Graphics& gfx) noxnd override
		{
			gfx.GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(slot, constantBuffer->GetGPUVirtualAddress());
		}

		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetConstantBuffer() noexcept
		{
			return constantBuffer;
		}

		[[nodiscard]] static std::shared_ptr<ConstantBuffer> Resolve(DEGraphics::Graphics& gfx, const std::string& tag,
			const C* constantData, size_t dateSize, const UINT slot)
		{
			return gfx.GetParent().Resolve<ConstantBuffer>(gfx, tag, constantData, dateSize, slot);
		}

		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}

		[[nodiscard]] static std::string GenerateUID_(const std::string& tag)
		{
			using namespace std::string_literals;
			return typeid(ConstantBuffer).name() + "#"s + tag;
		}

		[[nodiscard]] std::string GetUID() const noexcept override
		{
			return GenerateUID(tag);
		}
	protected:
		wrl::ComPtr<ID3D12Resource> constantBuffer;
		std::string tag;
		size_t bufferSize = 0u;
	private:
		UINT slot;
	};
}