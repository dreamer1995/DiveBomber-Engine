#pragma once
#include "GraphicsResource.h"
#include "WindowResource.h"
#include "FenceManager.h"

#include <queue>

class CommandManager final
{
public:
	CommandManager(ID3D12Device2* inputDevice, D3D12_COMMAND_LIST_TYPE intputType, std::shared_ptr<FenceManager> inputFenceManager);
	~CommandManager();
	ID3D12CommandQueue* GetCommandQueue() noexcept;

	void Signal() noexcept;
	void Flush() noexcept;

	wrl::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
	wrl::ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(ID3D12CommandAllocator* commandAllocator);

	ID3D12GraphicsCommandList2* GetCommandList();
	uint64_t ExcuteCommandList(ID3D12GraphicsCommandList2* commandList);
private:
	D3D12_COMMAND_LIST_TYPE type;
	wrl::ComPtr<ID3D12Device2> device;
	wrl::ComPtr<ID3D12CommandQueue> commandQueue;
	std::shared_ptr<FenceManager> fenceManager;
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

