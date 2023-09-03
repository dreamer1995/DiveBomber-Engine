#include "Material.h"

#include "..\Graphics.h"
#include "..\BindableObject\ConstantBuffer.h"
#include "..\BindableObject\Texture.h"

#include <iostream>

namespace DiveBomber::Component
{
    using namespace DEGraphics;
    using namespace BindableObject;

    Material::Material(Graphics& inputGfx, const std::wstring inputName)
        :
        gfx(inputGfx),
        name(inputName)
    {
        indexConstantBuffer = std::make_shared<ConstantBuffer<UINT>>(gfx, "TestSphereIndexConstant", 0u);
    }

    void Material::SetTexture(const std::shared_ptr<Texture> texture, UINT slot) noexcept
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

    void Material::SetConstant(const std::string constantName, const std::shared_ptr<DynamicConstantBufferInHeap> constant, UINT slot) noexcept
    {
        if (slot >= numConstantIndices)
        {
            UINT needInsert = slot - numConstantIndices + 1;
            std::vector<UINT>::iterator it = shaderResourceIndices.begin();
            shaderResourceIndices.insert(it + slot, needInsert, 0u);
            numConstantIndices = slot + 1;
        }
        shaderResourceIndices[slot] = constant->GetCBVDescriptorHeapOffset();
        dynamicConstantMap[constantName] = constant;
    }

    void Material::Bind(Graphics& gfx) noxnd
    {
        indexConstantBuffer->Update(gfx, shaderResourceIndices.data(), shaderResourceIndices.size() * sizeof(UINT));
        indexConstantBuffer->Bind(gfx);
    }

    std::wstring Material::GetName() const noexcept
    {
        return name;
    }

    //void Material::SetMaterialParameterScalar(std::string constantName, std::string key, float scalar) const noexcept
    //{
    //    auto it = dynamicConstantMap.find(constantName);
    //    if (it != dynamicConstantMap.end())
    //    {
    //        DynamicConstantProcess::Buffer buffer = it->second->GetBuffer();
    //        buffer[key] = scalar;
    //        it->second->Update(gfx, buffer);
    //    }
    //}

    //void Material::SetMaterialParameterVector(std::string constantName, std::string key, DirectX::XMFLOAT4 vector) const noexcept
    //{
    //    auto it = dynamicConstantMap.find(constantName);
    //    if (it != dynamicConstantMap.end())
    //    {
    //        auto buffer = it->second->GetBuffer();
    //        auto a = buffer[key];
    //        DirectX::XMFLOAT4 b = static_cast<dx::XMFLOAT4&>(a);
    //        std::cout << b.x << std::endl;
    //        //buffer[key] = vector;
    //        it->second->Update(gfx, buffer);
    //    }
    //}
}