#include "ResourceStateTracker.h"

#include "..\DX\CommandList.h"

//#pragma warning(disable:26110)

namespace DiveBomber::DX
{
    std::mutex ResourceStateTracker::globalResourceStateMutex;
    bool ResourceStateTracker::isLocked = false;
    std::unordered_map<ID3D12Resource*, ResourceStateTracker::ResourceState> ResourceStateTracker::globalResourceState;

    ResourceStateTracker::ResourceStateTracker()
    {
    }

    ResourceStateTracker::~ResourceStateTracker()
    {
    }

    void ResourceStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier)
    {
        if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
        {
            const D3D12_RESOURCE_TRANSITION_BARRIER& transitionBarrier = barrier.Transition;

            const auto iter = finalResourceState.find(transitionBarrier.pResource);
            if (iter != finalResourceState.end())
            {
                auto& resourceState = iter->second;

                if (transitionBarrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
                    !resourceState.subresourceState.empty())
                {
                    for (auto subresourceState : resourceState.subresourceState)
                    {
                        if (transitionBarrier.StateAfter != subresourceState.second)
                        {
                            D3D12_RESOURCE_BARRIER newBarrier = barrier;
                            newBarrier.Transition.Subresource = subresourceState.first;
                            newBarrier.Transition.StateBefore = subresourceState.second;
                            resourceBarriers.push_back(newBarrier);
                        }
                    }
                }
                else
                {
                    auto finalState = resourceState.GetSubresourceState(transitionBarrier.Subresource);
                    if (transitionBarrier.StateAfter != finalState)
                    {
                        D3D12_RESOURCE_BARRIER newBarrier = barrier;
                        newBarrier.Transition.StateBefore = finalState;
                        resourceBarriers.push_back(newBarrier);
                    }
                }
            }
            else
            {
                pendingResourceBarriers.push_back(barrier);
            }

            finalResourceState[transitionBarrier.pResource].SetSubresourceState(transitionBarrier.Subresource, transitionBarrier.StateAfter);
        }
    }

    void ResourceStateTracker::AddTransitionBarrier(const wrl::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource)
    {
        if (resource)
        {
            ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COMMON, stateAfter, subResource));
        }
    }

    void ResourceStateTracker::UAVBarrier(const wrl::ComPtr<ID3D12Resource> resource)
    {
        ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(resource.Get()));
    }

    void ResourceStateTracker::AliasBarrier(const wrl::ComPtr<ID3D12Resource> resourceBefore, const wrl::ComPtr<ID3D12Resource> resourceAfter)
    {
        ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Aliasing(resourceBefore.Get(), resourceAfter.Get()));
    }

    // https://www.3dgep.com/learning-directx-12-3/#Resource_State_Tracking
    uint32_t ResourceStateTracker::ExecutePendingResourceBarriers(CommandList& commandList)
    {
        assert(isLocked);

        std::vector<D3D12_RESOURCE_BARRIER> resolvedResourceBarriers;

        resolvedResourceBarriers.reserve(pendingResourceBarriers.size());

        for (auto pendingBarrier : pendingResourceBarriers)
        {
            if (pendingBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
            {
                auto pendingTransition = pendingBarrier.Transition;

                const auto& iter = globalResourceState.find(pendingTransition.pResource);
                if (iter != globalResourceState.end())
                {
                    auto& resourceState = iter->second;
                    if (pendingTransition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
                        !resourceState.subresourceState.empty())
                    {
                        for (auto subresourceState : resourceState.subresourceState)
                        {
                            if (pendingTransition.StateAfter != subresourceState.second)
                            {
                                D3D12_RESOURCE_BARRIER newBarrier = pendingBarrier;
                                newBarrier.Transition.Subresource = subresourceState.first;
                                newBarrier.Transition.StateBefore = subresourceState.second;
                                resolvedResourceBarriers.push_back(newBarrier);
                            }
                        }
                    }
                    else
                    {
                        auto globalState = (iter->second).GetSubresourceState(pendingTransition.Subresource);
                        if (pendingTransition.StateAfter != globalState)
                        {
                            pendingBarrier.Transition.StateBefore = globalState;
                            resolvedResourceBarriers.push_back(pendingBarrier);
                        }
                    }
                }
            }
        }

        UINT numBarriers = static_cast<UINT>(resolvedResourceBarriers.size());
        if (numBarriers > 0)
        {
            auto d3d12CommandList = commandList.GetGraphicsCommandList();
            d3d12CommandList->ResourceBarrier(numBarriers, resolvedResourceBarriers.data());
        }

        pendingResourceBarriers.clear();

        return numBarriers;
    }

    void ResourceStateTracker::ExecuteResourceBarriers(CommandList& commandList)
    {
        UINT numBarriers = static_cast<UINT>(resourceBarriers.size());
        if (numBarriers > 0)
        {
            auto d3d12CommandList = commandList.GetGraphicsCommandList();
            d3d12CommandList->ResourceBarrier(numBarriers, resourceBarriers.data());
            resourceBarriers.clear();
        }
    }

    void ResourceStateTracker::CommitFinalResourceStates()
    {
        assert(isLocked);

        for (const auto& resourceState : finalResourceState)
        {
            globalResourceState[resourceState.first] = resourceState.second;
        }

        finalResourceState.clear();
    }

    void ResourceStateTracker::Reset()
    {
        pendingResourceBarriers.clear();
        resourceBarriers.clear();
        finalResourceState.clear();
    }

    void ResourceStateTracker::Lock()
    {
        if (!isLocked)
        {
            globalResourceStateMutex.lock();
            isLocked = true;
        }
    }

    void ResourceStateTracker::Unlock()
    {
        if (isLocked)
        {
            globalResourceStateMutex.unlock();
            isLocked = false;
        }
    }

    void ResourceStateTracker::AddGlobalResourceState(const wrl::ComPtr<ID3D12Resource> resource, const D3D12_RESOURCE_STATES state)
    {

        if (resource != nullptr)
        {
            std::lock_guard<std::mutex> lock(globalResourceStateMutex);
            globalResourceState[resource.Get()].SetSubresourceState(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
        }
    }

    void ResourceStateTracker::RemoveGlobalResourceState(const wrl::ComPtr<ID3D12Resource> resource)
    {
        if (resource != nullptr)
        {
            std::lock_guard<std::mutex> lock(globalResourceStateMutex);
            globalResourceState.erase(resource.Get());
        }
    }

    ResourceStateTracker::ResourceState::ResourceState(D3D12_RESOURCE_STATES inputState)
        : state(inputState)
    {
    }

    void ResourceStateTracker::ResourceState::SetSubresourceState(UINT subresource, D3D12_RESOURCE_STATES inputState)
    {
        if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
        {
            state = inputState;
            subresourceState.clear();
        }
        else
        {
            subresourceState[subresource] = inputState;
        }
    }

    D3D12_RESOURCE_STATES ResourceStateTracker::ResourceState::GetSubresourceState(UINT subresource) const
    {
        D3D12_RESOURCE_STATES returnState = state;
        const auto iter = subresourceState.find(subresource);
        if (iter != subresourceState.end())
        {
            returnState = iter->second;
        }
        return returnState;
    }
}