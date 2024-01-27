#include "Viewport.h"

#include "..\..\GraphicsSource.h"

namespace DiveBomber::DEResource
{
    using namespace DEGraphics;
    using namespace DEException;

    Viewport::Viewport(const std::wstring& inputName)
        :
        Resource(inputName),
        viewport(CD3DX12_VIEWPORT(0.0f, 0.0f,
            static_cast<float>(Graphics::GetInstance().GetWidth()), static_cast<float>(Graphics::GetInstance().GetHeight())))
    {
    }

    Viewport::~Viewport()
    {
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

    void Viewport::Bind() noxnd
    {
        Graphics::GetInstance().GetGraphicsCommandList()->RSSetViewports(1, &viewport);
    }
}