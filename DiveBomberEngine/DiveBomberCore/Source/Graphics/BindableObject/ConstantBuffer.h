#pragma once
#include "Bindable.h"
#include "..\..\DiveBomberCore.h"
#include "..\Graphics.h"
#include "GlobalBindableManager.h"
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
		ConstantBuffer(const std::string& inputTag,
			const C& constantData, UINT inputSlot)
			:
			ConstantBuffer(inputTag, &constantData, sizeof(constantData), inputSlot)
		{
		}

		ConstantBuffer(const std::string& inputTag,
			const C* constantData, size_t inputdataSize, UINT inputSlot)
			:
			tag(inputTag),
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
				std::string outPutString = "Constant in " + tag + " has no data!";
				throw std::exception(outPutString.c_str());
			}
		}

		ConstantBuffer(const std::string& inputTag, UINT inputSlot)
			:
			tag(inputTag),
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

			// Create a committed resource for the GPU resource in a default heap.
			GFX_THROW_INFO_NAMESPACE(DEGraphics::Graphics::GetInstance().GetDevice()->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDes,
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(&constantBuffer)));

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

		void Bind() noxnd override
		{
			DEGraphics::Graphics::GetInstance().GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(slot, constantBuffer->GetGPUVirtualAddress());
		}

		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetConstantBuffer() noexcept
		{
			return constantBuffer;
		}

		[[nodiscard]] static std::shared_ptr<ConstantBuffer> Resolve(const std::string& tag,
			const C* constantData, size_t dataSize, const UINT slot)
		{
			return GlobalBindableManager::Resolve<ConstantBuffer>(tag, constantData, dataSize, slot);
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