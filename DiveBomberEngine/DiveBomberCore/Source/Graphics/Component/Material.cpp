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

    Material::Material(const std::wstring inputName)
        :
        name(inputName)
    {
        indexConstantBuffer = std::make_shared<ConstantBuffer<UINT>>("TestSphereIndexConstant", 0u);

        configFile = ProjectDirectoryW L"Asset\\Material\\" + name + L".json";

        LoadConfig();

        //LoadShader();
    }

    void DiveBomber::Component::Material::LoadConfig()
    {
        if (!fs::exists(configFile))
        {
            fs::path builtShaderDirectory(ProjectDirectoryW L"Asset\\Material\\");
            if (!fs::exists(builtShaderDirectory))
            {
                fs::create_directories(builtShaderDirectory);
            }

            CreateDefaultConfig(configFile);
        }
        else
        {
            std::ifstream rawFile(configFile);
            if (!rawFile.is_open())
            {
                throw std::exception("Unable to open script file");
            }
            rawFile >> config;

            configFileLastSaveTime = fs::last_write_time(configFile);

            std::wstring shaderName = Utility::ToWide(config.at("ShaderName"));
            
            fs::file_time_type builtLastSaveTime = Shader::GetSourceFileLastSaveTime(shaderName);
            if (builtLastSaveTime > configFileLastSaveTime)
            {
                UploadConfig(shaderName);
            }
        }
    }

    void Material::CreateDefaultConfig(fs::path configPath)
    {
        config["ShaderName"] = "TestShader";

        UploadConfig(Utility::ToWide(config.at("ShaderName")));

        // write prettified JSON to another file
        std::ofstream o(configPath);
        o << std::setw(4) << config << std::endl;

        configFileLastSaveTime = fs::last_write_time(configFile);
    }

    void Material::LoadShader()
    {
        std::wstring shaderName = Utility::ToWide(config.at("ShaderName"));

        for (auto& shaderStage : config.at("ShaderStage"))
        {
            std::cout << shaderStage << std::endl;
            
            std::shared_ptr<Shader> shader = Shader::Resolve(shaderName, shaderStage);
            ShaderManager::GetInstance().AddToUsingPool(shader);

            std::wstring paramsFile;
            ParseParamsFile(paramsFile);
        }
    }

    void Material::UploadConfig(const std::wstring shaderName)
    {
        std::wstring paramString = Shader::GetShaderParamsString(shaderName);

        json paramsData;
        paramsData = json::parse(paramString, nullptr, false);

        if (paramsData.is_discarded())
        {
            std::cout << "parse json error" << std::endl;
        }

        config["Stage"] = paramsData["Stage"];

        for (const auto& param : paramsData["Param"])
        {
            json materialData;

            bool existed = false;
            for (const auto& existedParam : config["Param"])
            {
                if (existedParam["Name"] == param["Name"])
                {
                    existed = true;
                    materialData = existedParam;
                    break;
                }
            }

            if (existed)
            {
                materialData["Name"] = param["Name"];
                materialData = param;
                materialData.erase("Default");

                auto defaultVal = param.find("Default");
                if (defaultVal != param.end())
                {
                    if (materialData["Type"] == "Texture")
                    {
                        if (param["Default"] == "Black")
                        {
                            materialData["Value"] = "WhiteTexture";
                        }
                        else if (param["Default"] == "White")
                        {
                            materialData["Value"] = "WhiteTexture";
                        }
                        else if (param["Default"] == "Normal")
                        {
                            materialData["Value"] = "WhiteTexture";
                        }
                    }
                    else
                    {
                        materialData["Value"] = param["Default"];
                    }
                }
                else
                {
                    if (materialData["Type"] == "Texture")
                    {
                        materialData["Value"] = "WhiteTexture";
                    }
                    else if (materialData["Type"] == "Float")
                    {
                        materialData["Value"] = 1.0;
                    }
                    else if (materialData["Type"] == "Color" || materialData["Type"] == "Float4")
                    {
                        materialData["Value"] = { 1.0f,1.0f,1.0f,1.0f };
                    }
                }

                config["Param"].emplace_back(materialData);
            }
            else
            {
                if()
            }

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

        for (const auto& param : paramsData["Param"])
        {
            auto a = param.find("enabled");
            if (a == param.end())
            {
                auto b = 1;
            }
            std::cout << param.at("Name") << std::endl;
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
}