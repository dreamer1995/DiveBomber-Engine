#pragma once
#include "DynamicConstant.h"
#include "ConstantBuffer.h"

namespace DiveBomber::GraphicResource
{
	class DynamicConstantBuffer : public ConstantBuffer<DynamicConstantProcess::Buffer>
	{
	public:
		DynamicConstantBuffer(const std::wstring& inputName,
			const DynamicConstantProcess::CookedLayout& inputLayout, UINT inputSlot = 999u)
			:
			ConstantBuffer<DynamicConstantProcess::Buffer>(inputName, inputSlot),
			dynamicBuffer(DynamicConstantProcess::Buffer(inputLayout))
		{
		}

		DynamicConstantBuffer(const std::wstring& inputName,
			const DynamicConstantProcess::Buffer& inputBuffer, UINT inputSlot = 999u)
			:
			DynamicConstantBuffer(inputName, inputBuffer.GetRootLayoutElement(), inputBuffer, inputSlot)
		{
		}

		DynamicConstantBuffer(const std::wstring& inputName,
			const DynamicConstantProcess::LayoutElement& inputLayout, const DynamicConstantProcess::Buffer& inputBuffer,
			UINT inputSlot = 999u)
			:
			ConstantBuffer<DynamicConstantProcess::Buffer>(inputName, inputSlot),
			dynamicBuffer(inputBuffer)
		{
			bufferSize = inputBuffer.GetSizeInBytes();
			if (bufferSize > 0)
			{
				Update(inputBuffer);
			}
		}

		~DynamicConstantBuffer()
		{
			DX::ResourceStateTracker::RemoveGlobalResourceState(constantBuffer);
		}

		virtual void Update(const DynamicConstantProcess::Buffer& buffer) override
		{
			// this will cost less? or more???
			if (&dynamicBuffer.GetRootLayoutElement() == &buffer.GetRootLayoutElement())
			{
				dynamicBuffer.CopyFrom(buffer);
			}
			else
			{
				dynamicBuffer = buffer;
			}

			size_t dataSize = dynamicBuffer.GetSizeInBytes();
			// Create an committed resource for the upload.
			if (dataSize > 0)
			{
				if (bufferSize != dataSize)
				{
					bufferSize = dataSize;
					InitializeConstantBufferSize();
				}

				std::shared_ptr<DX::CommandList> commandList = DEGraphics::Graphics::GetInstance().GetCommandList();

				std::shared_ptr<DX::UploadBufferAllocation> uploadBufferAllocation =
					commandList->AllocateDynamicUploadBuffer(bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

				D3D12_SUBRESOURCE_DATA subresourceData = {};
				subresourceData.pData = dynamicBuffer.GetData();
				subresourceData.RowPitch = bufferSize;
				subresourceData.SlicePitch = 1u;

				commandList->AddTransitionBarrier(constantBuffer, D3D12_RESOURCE_STATE_COPY_DEST, true);

				UpdateSubresources(commandList->GetGraphicsCommandList().Get(),
					constantBuffer.Get(), uploadBufferAllocation->resourceBuffer.Get(),
					uploadBufferAllocation->offset, 0, 1, &subresourceData);

				commandList->AddTransitionBarrier(constantBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, true);
			}
			else
			{
				std::string outPutString = "Constant in " + Utility::ToNarrow(name) + " has no data!";
				throw std::exception(outPutString.c_str());
			}
		}

		[[nodiscard]] const DynamicConstantProcess::Buffer& GetBuffer() const noexcept
		{
			return dynamicBuffer;
		}
	protected:
		DynamicConstantProcess::Buffer dynamicBuffer;
	};
}