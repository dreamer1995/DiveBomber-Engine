#include "Viewport.h"

#include "..\..\Exception\GraphicsException.h"
#include "d3dx12.h"

namespace DiveBomber::DX
{
    using namespace DEException;

    Viewport::Viewport()
    {
        viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(MainWindowWidth), static_cast<float>(MainWindowHeight));
    }

    D3D12_VIEWPORT Viewport::GetViewport() const noexcept
    {
        return viewport;
    }

    void Viewport::ResizeViewport(uint32_t width, uint32_t height) noxnd
    {
        width = std::max(1u, width);
        height = std::max(1u, height);
        viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    }

    void Viewport::Bind(wrl::ComPtr<ID3D12GraphicsCommandList2> commandList) noxnd
    {
        commandList->RSSetViewports(1, &viewport);
    }
}