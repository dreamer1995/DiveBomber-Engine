#pragma once
#include "..\GraphicsHeader.h"

#include <memory>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::DX
{
	class UploadBuffer;
	class ResourceStateTracker;

	class CommandList final
	{
	public:
		CommandList(wrl::ComPtr<ID3D12Device10> device, D3D12_COMMAND_LIST_TYPE inputType);
		~CommandList();
		void Reset();
		[[nodiscard]] wrl::ComPtr<ID3D12GraphicsCommandList7> GetGraphicsCommandList() const;
		void Close();
		[[nodiscard]] D3D12_COMMAND_LIST_TYPE GetCommandListType() const noexcept;

		void AddTransitionBarrier(const wrl::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES stateAfter, bool flushBarriers = false, UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		void UAVBarrier(const wrl::ComPtr<ID3D12Resource> resource, bool flushBarriers = false);
		void AliasingBarrier(const wrl::ComPtr<ID3D12Resource> beforeResource, const wrl::ComPtr<ID3D12Resource> afterResource, bool flushBarriers = false);
		void ExecuteResourceBarriers();
		[[nodiscard]] bool Close(CommandList& pendingCommandList);

		void TrackResource(wrl::ComPtr<ID3D12Object> object) noexcept;
		void ReleaseTrackedObjects() noexcept;
	private:
		D3D12_COMMAND_LIST_TYPE type;
		wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
		wrl::ComPtr<ID3D12GraphicsCommandList7> commandList;
		std::unique_ptr<UploadBuffer> uploadBuffer;
		std::unique_ptr<ResourceStateTracker> resourceStateTracker;
		std::vector<wrl::ComPtr<ID3D12Object>> trackedObjects;
	};
}

