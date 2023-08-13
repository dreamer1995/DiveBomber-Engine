#include "ResourceStateTracker.h"

#include "..\DX\CommandList.h"

namespace DiveBomber::Component
{
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
                // If the known final state of the resource is different...
                if (transitionBarrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
                    !resourceState.subresourceState.empty())
                {
                    // First transition all of the subresources if they are different than the StateAfter.
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
                        // Push a new transition barrier with the correct before state.
                        D3D12_RESOURCE_BARRIER newBarrier = barrier;
                        newBarrier.Transition.StateBefore = finalState;
                        resourceBarriers.push_back(newBarrier);
                    }
                }
            }
            else // In this case, the resource is being used on the command list for the first time. 
            {
                // Add a pending barrier. The pending barriers will be resolved
                // before the command list is executed on the command queue.
                pendingResourceBarriers.push_back(barrier);
            }

            // Push the final known state (possibly replacing the previously known state for the subresource).
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

    uint32_t ResourceStateTracker::ExecutePendingResourceBarriers(DX::CommandList& commandList)
    {
        assert(isLocked);

        // Resolve the pending resource barriers by checking the global state of the 
        // (sub)resources. Add barriers if the pending state and the global state do
        //  not match.
        std::vector<D3D12_RESOURCE_BARRIER> resolvedResourceBarriers;
        // Reserve enough space (worst-case, all pending barriers).
        resolvedResourceBarriers.reserve(pendingResourceBarriers.size());

        for (auto pendingBarrier : pendingResourceBarriers)
        {
            if (pendingBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)  // Only transition barriers should be pending...
            {
                auto pendingTransition = pendingBarrier.Transition;

                const auto& iter = globalResourceState.find(pendingTransition.pResource);
                if (iter != globalResourceState.end())
                {
                    // If all subresources are being transitioned, and there are multiple
                    // subresources of the resource that are in a different state...
                    auto& resourceState = iter->second;
                    if (pendingTransition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
                        !resourceState.subresourceState.empty())
                    {
                        // Transition all subresources
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
                        // No (sub)resources need to be transitioned. Just add a single transition barrier (if needed).
                        auto globalState = (iter->second).GetSubresourceState(pendingTransition.Subresource);
                        if (pendingTransition.StateAfter != globalState)
                        {
                            // Fix-up the before state based on current global state of the resource.
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

    void ResourceStateTracker::ExecuteResourceBarriers(DX::CommandList& commandList)
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

        // Commit final resource states to the global resource state array (map).
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
        globalResourceStateMutex.lock();
        isLocked = true;
    }

    void ResourceStateTracker::Unlock()
    {
        globalResourceStateMutex.unlock();
        isLocked = false;
    }

    void ResourceStateTracker::AddGlobalResourceState(wrl::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES state)
    {

        if (resource != nullptr)
        {
            std::lock_guard<std::mutex> lock(globalResourceStateMutex);
            globalResourceState[resource].SetSubresourceState(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
        }
    }

    void ResourceStateTracker::RemoveGlobalResourceState(wrl::ComPtr<ID3D12Resource> resource)
    {
        if (resource != nullptr)
        {
            std::lock_guard<std::mutex> lock(globalResourceStateMutex);
            globalResourceState.erase(resource);
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