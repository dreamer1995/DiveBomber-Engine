#include "CommandList.h"

#include "..\GraphicsSource.h"
#include "UploadBuffer.h"
#include "ResourceStateTracker.h"
#include "..\Resource\Resource.h"

namespace DiveBomber::DX
{
	using namespace DEGraphics;
	using namespace DEException;
	CommandList::CommandList(D3D12_COMMAND_LIST_TYPE inputType)
		:
		type(inputType)
	{
		auto device = Graphics::GetInstance().GetDevice();

		HRESULT hr;
		GFX_THROW_INFO(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));
		GFX_THROW_INFO(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
		
		uploadBuffer = std::make_unique<UploadBuffer>();
		resourceStateTracker = std::make_unique<ResourceStateTracker>();
	}

	CommandList::~CommandList()
	{
	}

	void CommandList::Reset()
	{
		HRESULT hr;
		GFX_THROW_INFO(commandAllocator->Reset());
		GFX_THROW_INFO(commandList->Reset(commandAllocator.Get(), nullptr));

		resourceStateTracker->Reset();
		ReleaseTracked();
		uploadBuffer->Reset();
	}

	wrl::ComPtr<ID3D12GraphicsCommandList7> CommandList::GetGraphicsCommandList() const
	{
		return commandList;
	}

	void CommandList::Close()
	{
		ExecuteResourceBarriers();
		HRESULT hr;
		GFX_THROW_INFO(commandList->Close());
	}

	D3D12_COMMAND_LIST_TYPE CommandList::GetCommandListType() const noexcept
	{
		return type;
	}

	void CommandList::AddTransitionBarrier(const wrl::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES stateAfter, bool flushBarriers, UINT subresource)
	{
		if (resource)
		{
			// The "before" state is not important. It will be resolved by the resource state tracker.
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COMMON, stateAfter, subresource);
			resourceStateTracker->ResourceBarrier(barrier);
		}

		if (flushBarriers)
		{
			ExecuteResourceBarriers();
		}
	}

	void CommandList::UAVBarrier(const wrl::ComPtr<ID3D12Resource> resource, bool flushBarriers)
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(resource.Get());

		resourceStateTracker->ResourceBarrier(barrier);

		if (flushBarriers)
		{
			ExecuteResourceBarriers();
		}
	}

	void CommandList::AliasingBarrier(const wrl::ComPtr<ID3D12Resource> beforeResource, const wrl::ComPtr<ID3D12Resource> afterResource, bool flushBarriers)
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(beforeResource.Get(), afterResource.Get());

		resourceStateTracker->ResourceBarrier(barrier);

		if (flushBarriers)
		{
			ExecuteResourceBarriers();
		}
	}

	void CommandList::ExecuteResourceBarriers()
	{
		resourceStateTracker->ExecuteResourceBarriers(*this);
	}

	bool CommandList::Close(CommandList& pendingCommandList)
	{
		ExecuteResourceBarriers();

		HRESULT hr;
		GFX_THROW_INFO(commandList->Close());

		uint32_t numPendingBarriers = resourceStateTracker->ExecutePendingResourceBarriers(pendingCommandList);

		resourceStateTracker->CommitFinalResourceStates();

		return numPendingBarriers > 0;
	}

	void CommandList::TrackResource(wrl::ComPtr<ID3D12Object> object) noexcept
	{
		trackedObjects.emplace_back(object);
	}

	void CommandList::TrackResource(std::shared_ptr<DEResource::Resource> resource) noexcept
	{
		trackedResource.emplace_back(resource);
	}

	void CommandList::TrackResource(std::shared_ptr<Component::Material> component) noexcept
	{
		trackedComponent.emplace_back(component);
	}

	void CommandList::ReleaseTracked() noexcept
	{
		trackedObjects.clear();
		trackedResource.clear();
		trackedComponent.clear();
	}

	std::shared_ptr<UploadBufferAllocation> CommandList::AllocateDynamicUploadBuffer(const size_t size, const size_t alignment)
	{
		return uploadBuffer->Allocate(size, alignment);
	}

	void CommandList::CopyResource(wrl::ComPtr<ID3D12Resource> dstRes, wrl::ComPtr<ID3D12Resource> srcRes)
	{
		AddTransitionBarrier(dstRes, D3D12_RESOURCE_STATE_COPY_DEST);
		AddTransitionBarrier(srcRes, D3D12_RESOURCE_STATE_COPY_SOURCE);

		ExecuteResourceBarriers();

		commandList->CopyResource(dstRes.Get(), srcRes.Get());
	}
}