#include "CommandQueue.h"

CommandQueue::CommandQueue(ID3D12Device2* device, std::shared_ptr<DxgiInfoManager> inputInfoManager)
{
    infoManager = inputInfoManager;
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    HRESULT hr;
    GFX_THROW_INFO(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)));
}