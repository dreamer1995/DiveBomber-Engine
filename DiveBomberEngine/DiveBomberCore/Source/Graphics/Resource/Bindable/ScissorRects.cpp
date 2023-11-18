#include "ScissorRects.h"


#include "..\..\GraphicsSource.h"

namespace DiveBomber::DEResource
{
    using namespace DEGraphics;
    using namespace DEException;

    ScissorRects::ScissorRects(const std::wstring& inputName)
        :
        Resource(inputName),
        scissorRects(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX))
    {
    }

    ScissorRects::~ScissorRects()
    {
    }

    D3D12_RECT ScissorRects::GetScissorRects() const noexcept
    {
        return scissorRects;
    }

    void ScissorRects::Bind() noxnd
    {
        Graphics::GetInstance().GetGraphicsCommandList()->RSSetScissorRects(1, &scissorRects);
    }
}