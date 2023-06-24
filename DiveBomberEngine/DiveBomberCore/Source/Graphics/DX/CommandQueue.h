#pragma once
#include "..\GraphicsResource.h"

#include <queue>

namespace DiveBomber::DX
{
	class CommandQueue final
	{
	public:
		CommandQueue(wrl::ComPtr<ID3D12Device2> inputDevice, D3D12_COMMAND_LIST_TYPE intputType = D3D12_COMMAND_LIST_TYPE_DIRECT);
		~CommandQueue();
		[[nodiscard]] wrl::ComPtr<ID3D12CommandQueue> GetCommandQueue() const noexcept;

		uint64_t Signal() noexcept;
		void Flush() noexcept;

		[[nodiscard]] wrl::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
		[[nodiscard]] wrl::ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(const wrl::ComPtr<ID3D12CommandAllocator> commandAllocator);

		[[nodiscard]] wrl::ComPtr<ID3D12GraphicsCommandList2> GetCommandList();
		[[nodiscard]] uint64_t ExecuteCommandList(wrl::ComPtr<ID3D12GraphicsCommandList2> commandList);
		void WaitForFenceValue(const uint64_t inputFenceValue) noexcept;
		[[nodiscard]] bool IsFenceComplete(uint64_t inputFenceValue) const noexcept;
	private:
		D3D12_COMMAND_LIST_TYPE type;
		wrl::ComPtr<ID3D12Device2> device;
		wrl::ComPtr<ID3D12CommandQueue> commandQueue;
		wrl::ComPtr<ID3D12Fence> fence;
		HANDLE fenceEvent;
		uint64_t fenceValue = 0;

		struct CommandAllocatorInfo
		{
			uint64_t fenceValue;
			wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
		};
		std::queue<CommandAllocatorInfo> commandAllocatorQueue;
		std::queue<wrl::ComPtr<ID3D12GraphicsCommandList2>> commandListQueue;
	};
}