#include "Material.h"

#include "..\Graphics.h"
#include "..\BindableObject\ConstantBuffer.h"
#include "..\BindableObject\Texture.h"

namespace DiveBomber::Component
{
    using namespace DEGraphics;
    using namespace BindableObject;

    Material::Material()
    {
    }

    void Material::AddTexture(const std::shared_ptr<Texture> texture, UINT slot) noexcept
    {
        assert(bindableTextureMap.find(slot) == bindableTextureMap.end());
        bindableTextureMap.emplace(slot, texture);
    }

    void Material::Bind(Graphics& gfx) noxnd
    {
        for (auto& bindableTexture : bindableTextureMap)
        {
            //bindableTexture.second->GetSRVDescriptorHeapOffset();
        }
    }
}