#pragma once

#include "..\GraphicsHeader.h"

#include <mutex>
#include <map>
#include <unordered_map>
#include <vector>

namespace DiveBomber::DX
{
	class CommandList;
	class DescriptorAllocation;
	class DescriptorAllocatorPage;

	class ResourceStateTracker final
	{
	public:
		ResourceStateTracker();
		~ResourceStateTracker();

		void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);
		void AddTransitionBarrier(const wrl::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		void UAVBarrier(const wrl::ComPtr<ID3D12Resource> resource = nullptr);
		void AliasBarrier(const wrl::ComPtr<ID3D12Resource> resourceBefore = nullptr, const wrl::ComPtr<ID3D12Resource> resourceAfter = nullptr);
		[[nodiscard]] uint32_t ExecutePendingResourceBarriers(CommandList& commandList);
		void ExecuteResourceBarriers(CommandList& commandList);
		void CommitFinalResourceStates();
		void Reset();

		static void Lock();
		static void Unlock();
		static void AddGlobalResourceState(const wrl::ComPtr<ID3D12Resource> resource, const D3D12_RESOURCE_STATES state);
		static void RemoveGlobalResourceState(const wrl::ComPtr<ID3D12Resource> resource);

	private:
        struct ResourceState
        {
            explicit ResourceState(D3D12_RESOURCE_STATES inputState = D3D12_RESOURCE_STATE_COMMON);

			void SetSubresourceState(UINT subresource, D3D12_RESOURCE_STATES inputState);
			[[nodiscard]] D3D12_RESOURCE_STATES GetSubresourceState(UINT subresource) const;
            

            D3D12_RESOURCE_STATES state;
            std::map<UINT, D3D12_RESOURCE_STATES> subresourceState;
        };

	private:
		std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;
		std::vector<D3D12_RESOURCE_BARRIER> pendingResourceBarriers;
		std::unordered_map<ID3D12Resource*, ResourceState> finalResourceState;

		static std::unordered_map<ID3D12Resource*, ResourceState> globalResourceState;

		static std::mutex globalResourceStateMutex;
		static bool isLocked;
	};
}