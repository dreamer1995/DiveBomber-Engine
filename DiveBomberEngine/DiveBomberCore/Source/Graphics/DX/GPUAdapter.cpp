#include "GPUAdapter.h"

namespace DiveBomber::DX
{
    using namespace DEException;

    GPUAdapter::GPUAdapter()
    {
        wrl::ComPtr<IDXGIFactory4> dxgiFactory;
        UINT createFactoryFlags = 0;
        HRESULT hr;
#if defined(_DEBUG)
        createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        wrl::ComPtr<ID3D12Debug> debugController;
        GFX_THROW_INFO(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
#endif
        // Poco::NotFoundException at memory location after Nvdia driver 531.18
        // Error 20 (this feature has not been implemented yet) in function AVolute::GetProductInfoT::<lambda_3920e95365a48b95dd51020986e9e351>::operator ()
        // caused by realtek
        GFX_THROW_INFO(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

        wrl::ComPtr<IDXGIAdapter1> dxgiAdapter1;
        // wrl::ComPtr<IDXGIAdapter4> dxgiAdapter4;

        if (UseWARP)
        {
            GFX_THROW_INFO(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
            GFX_THROW_INFO(dxgiAdapter1.As(&dxgiAdapter));
        }
        else
        {
            SIZE_T maxDedicatedVideoMemory = 0;
            for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
            {
                DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
                dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

                // Check to see if the adapter can create a D3D12 device without actually 
                // creating it. The adapter with the largest dedicated video memory
                // is favored.
                if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                    SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
                        D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
                    dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
                {
                    maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                    GFX_THROW_INFO(dxgiAdapter1.As(&dxgiAdapter));
                }
            }
        }
    }

    wrl::ComPtr<IDXGIAdapter4> GPUAdapter::GetAdapter() const noexcept
    {
        return dxgiAdapter;
    }
}