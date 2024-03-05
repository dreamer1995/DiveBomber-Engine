#pragma once
#include "..\..\Resource.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsExceptionEX.h"
#include "..\DX\CommandList.h"
#include "..\DX\ResourceStateTracker.h"
#include "..\DX\UploadBuffer.h"

namespace DiveBomber::GraphicResource
{
	template<typename C>
	class ConstantBuffer : public DiveBomber::Resource
	{
	public:
		ConstantBuffer(const std::wstring& inputName,
			const C& constantData, UINT inputSlot)
			:
			ConstantBuffer(inputName, &constantData, sizeof(constantData), inputSlot)
		{
		}

		ConstantBuffer(const std::wstring& inputName,
			const C* constantData, size_t inputdataSize, UINT inputSlot)
			:
			Resource(inputName),
			slot(inputSlot),
			bufferSize(inputdataSize)
		{
			if (bufferSize > 0)
			{
				InitializeConstantBufferSize();
				Update(constantData, bufferSize);
			}
			else
			{
				std::string outPutString = "Constant in " + Utility::ToNarrow(name) + " has no data!";
				throw std::exception(outPutString.c_str());
			}
		}

		ConstantBuffer(const std::wstring& inputName, UINT inputSlot)
			:
			Resource(inputName),
			slot(inputSlot)
		{
			InitializeConstantBufferSize();
		}

		~ConstantBuffer()
		{
			DX::ResourceStateTracker::RemoveGlobalResourceState(constantBuffer);
		}

		void InitializeConstantBufferSize()
		{
			HRESULT hr;

			size_t initBufferSize = bufferSize != 0 ? bufferSize : 1u;
			initBufferSize = Utility::AlignUp(initBufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
			const CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_DEFAULT };
			const CD3DX12_RESOURCE_DESC resDes = CD3DX12_RESOURCE_DESC::Buffer(initBufferSize, D3D12_RESOURCE_FLAG_NONE);

			// Track once for safety.
			DEGraphics::Graphics::GetInstance().GetCommandList()->TrackResource(constantBuffer);

			// Create a committed resource for the GPU resource in a default heap.
			GFX_THROW_INFO_NAMESPACE(DEGraphics::Graphics::GetInstance().GetDevice()->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDes,
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(&constantBuffer)));

			constantBuffer->SetName(name.c_str());

			DX::ResourceStateTracker::AddGlobalResourceState(constantBuffer, D3D12_RESOURCE_STATE_COMMON);
		}

		virtual void Update(const C& constantData)
		{
			Update(&constantData, sizeof(constantData));
		}

		virtual void Update(const C* constantData, size_t dataSize)
		{
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
				subresourceData.pData = constantData;
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

		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetConstantBuffer() noexcept
		{
			return constantBuffer;
		}

		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::wstring& name, Ignore&&...ignore)
		{
			using namespace std::string_literals;
			return typeid(ConstantBuffer).name() + "#"s + Utility::ToNarrow(name);
		}

		[[nodiscard]] std::string GetUID() const noexcept override
		{
			return GenerateUID(name);
		}
	protected:
		wrl::ComPtr<ID3D12Resource> constantBuffer;
		size_t bufferSize = 0u;
		UINT slot;
	};
}