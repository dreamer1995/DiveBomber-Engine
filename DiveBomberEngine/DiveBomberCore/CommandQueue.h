#pragma once
#include "GraphicsResource.h"

class CommandQueue
{
public:
	CommandQueue(ID3D12Device2* device, std::shared_ptr<DxgiInfoManager> inputInfoManager);
	ID3D12CommandQueue* GetDecive();
private:
	wrl::ComPtr<ID3D12CommandQueue> commandQueue;
	std::shared_ptr<DxgiInfoManager> infoManager;
};
