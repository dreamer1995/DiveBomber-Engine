#include "Material.h"

#include "..\Graphics.h"
#include "..\BindableObject\ConstantBuffer.h"
#include "..\BindableObject\Texture.h"

namespace DiveBomber::Component
{
    using namespace DEGraphics;
    using namespace BindableObject;

    Material::Material(Graphics& gfx)
    {
        indexConstantBuffer = std::make_shared<ConstantBuffer<IndexConstant>>(gfx, "TestSphereIndexConstant", indexConstant, 0u);
    }

    void Material::AddTexture(const std::shared_ptr<Texture> texture, UINT slot) noexcept
    {
        indexConstant.texureIndex[slot] = texture->GetSRVDescriptorHeapOffset();
    }

    void Material::Bind(Graphics& gfx) noxnd
    {
        indexConstantBuffer->Update(gfx, indexConstant);
        indexConstantBuffer->Bind(gfx);
    }
}