#pragma once
#include "..\GraphicsHeader.h"
#include "..\..\..\ThirdParty\ThreadSafeQueue.h"

#include <queue>
#include <memory>
//#include <atomic>               // For std::atomic_bool
//#include <condition_variable>   // For std::condition_variable.

namespace DiveBomber::DX
{
	class CommandList;

	class CommandQueue final
	{
	public:
		CommandQueue(wrl::ComPtr<ID3D12Device2> inputDevice, D3D12_COMMAND_LIST_TYPE intputType = D3D12_COMMAND_LIST_TYPE_DIRECT);
		~CommandQueue();
		[[nodiscard]] wrl::ComPtr<ID3D12CommandQueue> GetCommandQueue() const noexcept;

		uint64_t Signal() noexcept;
		void Flush() noexcept;

		[[nodiscard]] std::shared_ptr<CommandList> GetCommandList();
		[[nodiscard]] uint64_t ExecuteCommandList(std::shared_ptr<CommandList> commandList);
		[[nodiscard]] uint64_t ExecuteCommandLists(const std::vector<std::shared_ptr<CommandList>>& commandLists);
		void WaitForFenceValue(const uint64_t inputFenceValue) noexcept;
		[[nodiscard]] bool IsFenceComplete(uint64_t inputFenceValue) const noexcept;
		void CommandListsGarbageCollection();
	private:
		D3D12_COMMAND_LIST_TYPE type;
		wrl::ComPtr<ID3D12Device2> device;
		wrl::ComPtr<ID3D12CommandQueue> commandQueue;
		wrl::ComPtr<ID3D12Fence> fence;
		std::atomic_uint64_t fenceValue = 0;

		ThirdParty::ThreadSafeQueue<std::tuple<uint64_t, std::shared_ptr<CommandList>>> inFlightCommandLists;
		ThirdParty::ThreadSafeQueue<std::shared_ptr<CommandList>> availableCommandLists;

		std::atomic_bool processCommandListsGarbageCollection = true;
		std::thread commandListsGarbageCollectionThread;
		std::mutex commandListsGarbageCollectionThreadMutex;
		// too long?
		std::condition_variable commandListsGarbageCollectionThreadConditionVariable;
	};
}