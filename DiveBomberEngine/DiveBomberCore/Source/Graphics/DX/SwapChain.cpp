#include "SwapChain.h"

#include "..\BindObj\RenderTarget.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::DX
{
    using namespace DEException;
    using namespace DX;
    using namespace BindObj;

    SwapChain::SwapChain(const HWND hWnd, const wrl::ComPtr<ID3D12CommandQueue> commandQueue)
    {
        wrl::ComPtr<IDXGIFactory4> dxgiFactory4;
        UINT createFactoryFlags = 0;
#if defined(_DEBUG)
        createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

        HRESULT hr;
        GFX_THROW_INFO(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = MainWindowWidth;
        swapChainDesc.Height = MainWindowHeight;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc = { 1, 0 };
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = SwapChainBufferCount;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        // It is recommended to always allow tearing if tearing support is available.
        swapChainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

        wrl::ComPtr<IDXGISwapChain1> swapChain1;
        GFX_THROW_INFO(dxgiFactory4->CreateSwapChainForHwnd(
            commandQueue.Get(),
            hWnd/*nullptr*/,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1));

        // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
        // will be handled manually.
        GFX_THROW_INFO(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

        GFX_THROW_INFO(swapChain1.As(&swapChain));
    }

    bool SwapChain::CheckTearingSupport()
    {
        BOOL allowTearing = FALSE;

        // Rather than create the DXGI 1.5 factory interface directly, we create the
        // DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
        // graphics debugging tools which will not support the 1.5 factory interface 
        // until a future update.
        wrl::ComPtr<IDXGIFactory4> factory4;
        if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
        {
            wrl::ComPtr<IDXGIFactory5> factory5;
            if (SUCCEEDED(factory4.As(&factory5)))
            {
                if (FAILED(factory5->CheckFeatureSupport(
                    DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                    &allowTearing, sizeof(allowTearing))))
                {
                    allowTearing = FALSE;
                }
            }
        }

        return allowTearing == TRUE;
    }

    wrl::ComPtr<IDXGISwapChain4> SwapChain::GetSwapChain() noexcept
    {
        return swapChain;
    }

    void SwapChain::UpdateBackBuffer(const wrl::ComPtr<ID3D12Device10> device,
        std::shared_ptr<DescriptorAllocator> descriptorAllocator)
    {
        if(!rtvDescHeaps)
            rtvDescHeaps = descriptorAllocator->Allocate(SwapChainBufferCount);

        for (int i = 0; i < SwapChainBufferCount; ++i)
        {
            wrl::ComPtr<ID3D12Resource> backBuffer;
            HRESULT hr;
            GFX_THROW_INFO(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

            std::shared_ptr<RenderTarget> renderTarget;
            renderTarget = std::make_shared<RenderTarget>(device, backBuffer, rtvDescHeaps, i);
            backBuffers[i] = renderTarget;
        }
    }

    std::shared_ptr<RenderTarget> SwapChain::GetCurrentBackBuffer() noexcept
    {
        const int currentIndex = swapChain->GetCurrentBackBufferIndex();
        return backBuffers[currentIndex];
    }

    wrl::ComPtr<ID3D12Resource> SwapChain::GetBackBuffer(const int i) noexcept
    {
        assert(i < SwapChainBufferCount);
        return backBuffers[i]->GetRenderTargetBuffer();
    }

    void SwapChain::ResetBackBuffer() noexcept
    {
        for (int i = 0; i < SwapChainBufferCount; ++i)
        {
            backBuffers[i].reset();
        }
    }

    void SwapChain::ResetSizeBackBuffer(const wrl::ComPtr<ID3D12Device10> device,
        const uint32_t inputWidth, const uint32_t inputHeight,
        std::shared_ptr<DescriptorAllocator> descriptorAllocator)
    {
        // Any references to the back buffers must be released
		// before the swap chain can be resized.
        ResetBackBuffer();

        // Don't allow 0 size swap chain back buffers.
        uint32_t width = std::max(1u, inputWidth);
        uint32_t height = std::max(1u, inputHeight);

        HRESULT hr;
        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        GFX_THROW_INFO(GetSwapChain()->GetDesc(&swapChainDesc));
        GFX_THROW_INFO(GetSwapChain()->ResizeBuffers(SwapChainBufferCount, width, height,
            swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

        UpdateBackBuffer(device, descriptorAllocator);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetBackBufferDescriptorHandle() const noexcept
    {
        const int currentIndex = swapChain->GetCurrentBackBufferIndex();
        return backBuffers[currentIndex]->GetDescriptorHandle();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetBackBufferDescriptorHandle(int i) const noexcept
    {
        return backBuffers[i]->GetDescriptorHandle();
    }
}