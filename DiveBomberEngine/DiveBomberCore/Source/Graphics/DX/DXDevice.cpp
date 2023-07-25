#include "DXDevice.h"

#include "..\..\Exception\GraphicsException.h"

#pragma comment(lib,"dxgi.lib")

namespace DiveBomber::DX
{
    using namespace DEException;

    DXDevice::DXDevice(const wrl::ComPtr<IDXGIAdapter4> adapter)
    {
        HRESULT hr;
        GFX_THROW_INFO(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dxDevice)));

        // Enable debug messages in debug mode.
#if defined(_DEBUG)
        wrl::ComPtr<ID3D12InfoQueue> pInfoQueue;
        if (SUCCEEDED(dxDevice.As(&pInfoQueue)))
        {
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

            // Suppress whole categories of messages
            //D3D12_MESSAGE_CATEGORY Categories[] = {};

            // Suppress messages based on their severity level
            D3D12_MESSAGE_SEVERITY Severities[] =
            {
                D3D12_MESSAGE_SEVERITY_INFO
            };

            // Suppress individual messages by their ID
            D3D12_MESSAGE_ID DenyIds[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
            };

            D3D12_INFO_QUEUE_FILTER NewFilter = {};
            //NewFilter.DenyList.NumCategories = _countof(Categories);
            //NewFilter.DenyList.pCategoryList = Categories;
            NewFilter.DenyList.NumSeverities = _countof(Severities);
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs = _countof(DenyIds);
            NewFilter.DenyList.pIDList = DenyIds;

            GFX_THROW_INFO(pInfoQueue->PushStorageFilter(&NewFilter));
        }
#endif
    }

    wrl::ComPtr<ID3D12Device2> DXDevice::GetDecive() const noexcept
    {
        return dxDevice;
    }
}