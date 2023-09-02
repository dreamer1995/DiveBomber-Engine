#pragma once
#include "Bindable.h"
#include "..\..\DiveBomberCore.h"
#include "..\Graphics.h"
#include "GlobalBindableManager.h"
#include "..\..\Exception\GraphicsExceptionEX.h"
#include "..\DX\CommandList.h"
#include "..\DX\ResourceStateTracker.h"
#include "..\DX\UploadBuffer.h"
#include "DynamicConstant.h"

namespace DiveBomber::BindableObject
{
	class DynamicConstantBuffer : public Bindable
	{
	public:
		DynamicConstantBuffer(DEGraphics::Graphics& gfx, const std::string& inputTag,
			const DynamicConstantProcess::LayoutElement& inputLayout, UINT inputSlot)
			:
			DynamicConstantBuffer(gfx, inputTag, inputLayout, nullptr, inputSlot)
		{
		}

		DynamicConstantBuffer(DEGraphics::Graphics& gfx, const std::string& inputTag,
			const DynamicConstantProcess::Buffer* inputBuffer, UINT inputSlot)
			:
			DynamicConstantBuffer(gfx, inputTag, inputBuffer->GetRootLayoutElement(), inputBuffer, inputSlot)
		{
		}

		DynamicConstantBuffer(DEGraphics::Graphics& gfx, const std::string& inputTag,
			const DynamicConstantProcess::LayoutElement& inputLayout, const DynamicConstantProcess::Buffer* inputBuffer,
			UINT inputSlot)
			:
			tag(inputTag),
			slot(inputSlot)
		{
			bufferSize = inputLayout.GetSizeInBytes();
			InitializeConstantBufferSize(gfx);
			if (bufferSize > 0)
			{
				Update(gfx, inputBuffer, bufferSize);
			}
		}

		~DynamicConstantBuffer()
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

		virtual void Update(DEGraphics::Graphics& gfx, const DynamicConstantProcess::Buffer* buffer)
		{
			Update(gfx, buffer, buffer->GetSizeInBytes());
		}

		virtual void Update(DEGraphics::Graphics& gfx, const DynamicConstantProcess::Buffer* buffer, size_t dataSize)
		{
			// Create an committed resource for the upload.
			if (dataSize > 0)
			{
				if (bufferSize != dataSize)
				{
					bufferSize = dataSize;
					InitializeConstantBufferSize(gfx);
				}

				std::shared_ptr<DX::CommandList> commandList = gfx.GetCommandList();

				std::shared_ptr<DX::UploadBufferAllocation> uploadBufferAllocation =
					commandList->AllocateDynamicUploadBuffer(bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

				D3D12_SUBRESOURCE_DATA subresourceData = {};
				subresourceData.pData = buffer->GetData();
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

		[[nodiscard]] static std::shared_ptr<DynamicConstantBuffer> Resolve(DEGraphics::Graphics& gfx, const std::string& tag,
			const DynamicConstantProcess::LayoutElement& layout, const DynamicConstantProcess::Buffer* buffer,
			UINT slot)
		{
			return gfx.GetParent().GetGlobalBindableManager()->Resolve<DynamicConstantBuffer>(gfx, tag, layout, buffer, slot);
		}

		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}

		[[nodiscard]] static std::string GenerateUID_(const std::string& tag)
		{
			using namespace std::string_literals;
			return typeid(DynamicConstantBuffer).name() + "#"s + tag;
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