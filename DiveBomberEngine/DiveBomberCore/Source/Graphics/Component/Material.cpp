#include "Material.h"

#include "..\Graphics.h"
#include "..\BindableObject\ConstantBuffer.h"
#include "..\BindableObject\Texture.h"
#include "..\BindableObject\Shader.h"
#include "..\..\Utility\GlobalParameters.h"
#include "..\DX\ShaderManager.h"

#include <iostream>
#include <fstream>

namespace DiveBomber::Component
{
    using namespace DEGraphics;
    using namespace BindableObject;
    using namespace DX;
    namespace fs = std::filesystem;

    Material::Material(const std::wstring inputName)
        :
        name(inputName)
    {
        indexConstantBuffer = std::make_shared<ConstantBuffer<UINT>>("TestSphereIndexConstant", 0u);

        LoadConfig();

        //LoadShader();
    }

    void DiveBomber::Component::Material::LoadConfig()
    {
        fs::path configFilePath(ProjectDirectoryW L"Asset\\Material\\" + name + L".json");

        if (!fs::exists(configFilePath))
        {
            fs::path builtShaderDirectory(ProjectDirectoryW L"Asset\\Material\\");
            if (!fs::exists(builtShaderDirectory))
            {
                fs::create_directories(builtShaderDirectory);
            }
            config = CreateDefaultConfig(configFilePath.wstring());
        }
        else
        {
            std::ifstream configFile(configFilePath);
            if (!configFile.is_open())
            {
                throw std::exception("Unable to open script file");
            }
            configFile >> config;
        }
    }

    json Material::CreateDefaultConfig(const std::wstring path)
    {
        json newConfig;
        newConfig["ShaderPath"] = "TestShader";
        newConfig["ShaderStage"] = { "VS","PS" };

        // write prettified JSON to another file
        std::ofstream o(path);
        o << std::setw(4) << newConfig << std::endl;

        return newConfig;
    }

    void Material::LoadShader()
    {
        for (auto& shaderStage : config.at("ShaderStage"))
        {
            std::shared_ptr<Shader> vertexShader = Shader::Resolve(L"TestShader", shaderStage);
            std::wstring paramsFile = vertexShader->LoadShaderBlob();
            ShaderManager::GetInstance().AddToUsingPool(vertexShader);
        }
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

    void Material::Bind() noxnd
    {
        indexConstantBuffer->Update(shaderResourceIndices.data(), shaderResourceIndices.size() * sizeof(UINT));
        indexConstantBuffer->Bind();
    }

    std::wstring Material::GetName() const noexcept
    {
        return name;
    }

    void Material::SetMaterialParameterScalar(std::string constantName, std::string key, float scalar) const noexcept
    {
        auto it = dynamicConstantMap.find(constantName);
        if (it != dynamicConstantMap.end())
        {
            DynamicConstantProcess::Buffer buffer = it->second->GetBuffer();
            buffer[key] = scalar;
            it->second->Update(buffer);
        }
    }

    void Material::SetMaterialParameterVector(std::string constantName, std::string key, DirectX::XMFLOAT4 vector) const noexcept
    {
        auto it = dynamicConstantMap.find(constantName);
        if (it != dynamicConstantMap.end())
        {
            auto buffer = it->second->GetBuffer();
            auto a = buffer[key];
            DirectX::XMFLOAT4 b = static_cast<dx::XMFLOAT4&>(a);
            buffer[key] = vector;
            it->second->Update(buffer);
        }
    }

    void DiveBomber::Component::Material::ParseParamsFile(const std::wstring paramsFile)
    {
        json paramsData;
        paramsData = json::parse(paramsFile, nullptr, false);

        if (paramsData.is_discarded())
        {
            std::cout << "parse json error" << std::endl;
        }

        for (const auto& param : paramsData.at("Param"))
        {
            auto a = param.find("enabled");
            if (a == param.end())
            {
                auto b = 1;
            }
            std::cout << param.at("Name") << std::endl;
        }
    }
}