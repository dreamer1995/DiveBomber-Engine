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
        indexConstantBuffer = std::make_shared<ConstantBuffer<UINT>>(gfx, "TestSphereIndexConstant", 0u);
    }

    void Material::AddTexture(const std::shared_ptr<Texture> texture, UINT slot) noexcept
    {
        if (slot >= numTextureIndices)
        {
            UINT needInsert = slot - numTextureIndices + 1;
            std::vector<UINT>::iterator it = shaderResourceIndices.end();
            shaderResourceIndices.insert(it, needInsert, 0u);
            numTextureIndices = slot + 1;
        }

        UINT index = (numConstantIndices + slot);
        shaderResourceIndices[index] = texture->GetSRVDescriptorHeapOffset();
    }

    void Material::AddConstant(const std::shared_ptr<DynamicConstantBufferInHeap> constant, UINT slot) noexcept
    {
        if (slot >= numConstantIndices)
        {
            UINT needInsert = slot - numConstantIndices + 1;
            std::vector<UINT>::iterator it = shaderResourceIndices.begin();
            shaderResourceIndices.insert(it + slot, needInsert, 0u);
            numConstantIndices = slot + 1;
        }
        shaderResourceIndices[slot] = constant->GetCBVDescriptorHeapOffset();
    }

    void Material::Bind(Graphics& gfx) noxnd
    {
        indexConstantBuffer->Update(gfx, shaderResourceIndices.data(), shaderResourceIndices.size() * sizeof(UINT));
        indexConstantBuffer->Bind(gfx);
    }
}