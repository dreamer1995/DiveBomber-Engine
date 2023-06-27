#include "ScissorRects.h"

namespace DiveBomber::DX
{
    using namespace DEException;

    ScissorRects::ScissorRects()
    {
        scissorRects = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);;
    }

    D3D12_RECT ScissorRects::GetScissorRects() const noexcept
    {
        return scissorRects;
    }

    void ScissorRects::Bind(wrl::ComPtr<ID3D12GraphicsCommandList2> commandList) noxnd
    {
        commandList->RSSetScissorRects(1, &scissorRects);
    }
}