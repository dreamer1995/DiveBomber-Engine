#include "Material.h"

#include "..\Graphics.h"
#include "..\BindableObject\ConstantBuffer.h"
#include "..\BindableObject\Texture.h"
#include "..\BindableObject\Shader.h"
#include "..\BindableObject\DynamicConstantBufferInHeap.h"
#include "..\..\Utility\GlobalParameters.h"
#include "..\DX\ShaderManager.h"
#include "..\DX\CommandQueue.h"

#include <iostream>
#include <fstream>
#include <any>

namespace DiveBomber::Component
{
    using namespace DEGraphics;
    using namespace BindableObject;
    using namespace DX;

    Material::Material(const std::wstring inputName)
        :
        name(inputName)
    {
        using namespace std::string_literals;
        indexConstantBuffer = std::make_shared<ConstantBuffer<UINT>>(Utility::ToNarrow(name) + "#"s + "IndexConstant", 4u);

        configFile = ProjectDirectoryW L"Asset\\Material\\" + name + L".json";

        GetConfig();
        ReloadConfig();
    }

    void DiveBomber::Component::Material::GetConfig()
    {
        if (!fs::exists(configFile))
        {
            fs::path builtShaderDirectory(ProjectDirectoryW L"Asset\\Material\\");
            if (!fs::exists(builtShaderDirectory))
            {
                fs::create_directories(builtShaderDirectory);
            }

            CreateDefaultConfig();
        }
        else
        {
            std::ifstream rawFile(configFile);
            if (!rawFile.is_open())
            {
                throw std::exception("Unable to open script file");
            }
            rawFile >> config;
            rawFile.close();

            configFileLastSaveTime = fs::last_write_time(configFile);

            std::wstring shaderName = Utility::ToWide(config["ShaderName"]);
            
            fs::file_time_type builtLastSaveTime = Shader::GetSourceFileLastSaveTime(shaderName);
            if (builtLastSaveTime > configFileLastSaveTime)
            {
                UploadConfig(shaderName);
            }
        }
    }

    void Material::CreateDefaultConfig()
    {
        config["ShaderName"] = "TestShader";

        UploadConfig(Utility::ToWide(config["ShaderName"]));
    }

    int Material::ParamTypeStringToEnum(std::string string) noexcept
    {
        if (string == "Float")
        {
            return (int)ShaderParamType::SPT_Float;
        }
        else if (string == "Float4")
        {
            return (int)ShaderParamType::SPT_Float4;
        }
        else if (string == "Color")
        {
            return (int)ShaderParamType::SPT_Color;
        }
        else if (string == "Texture")
        {
            return (int)ShaderParamType::SPT_Texture;
        }
        else
        {
            std::cout << "Unknow parameter type " + string + " in shader!" << std::endl;
            return -1;
        }
    }

    int Material::ShaderStageStringToEnum(std::string string) noexcept
    {
        if (string == "VS")
        {
            return (int)ShaderType::VertexShader;
        }
        else if (string == "HS")
        {
            return (int)ShaderType::HullShader;
        }
        else if (string == "DS")
        {
            return (int)ShaderType::DomainShader;
        }
        else if (string == "GS")
        {
            return (int)ShaderType::VertexShader;
        }
        else if (string == "PS")
        {
            return (int)ShaderType::PixelShader;
        }
        else if (string == "CS")
        {
            return (int)ShaderType::ComputeShader;
        }
        else
        {
            std::cout << "Unknow shader stage: " + string + "!" << std::endl;
            return -1;
        }
    }

    int Material::ParamTypeToDynamicConstantType(ShaderParamType type) noexcept
    {
        switch (type)
        {
        case ShaderParamType::SPT_Float:
            return DynamicConstantProcess::Float;
        case ShaderParamType::SPT_Color:
        case ShaderParamType::SPT_Float4:
            return DynamicConstantProcess::Float4;
        case ShaderParamType::SPT_Bool:
            return DynamicConstantProcess::Bool;
        default:
            std::cout << "Unknow shader param type: " + std::to_string((int)type) + "!" << std::endl;
            return -1;
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

        for (int i = 0; i < paramsData["Param"].size(); i++)
        {
            paramsData["Param"][i]["Type"] = ParamTypeStringToEnum(paramsData["Param"][i]["Type"]);
        }
        
        for (int i = 0; i < paramsData["Stage"].size(); i++)
        {
            paramsData["Stage"][i] = ShaderStageStringToEnum(paramsData["Stage"][i]);
        }

        config["Stage"] = paramsData["Stage"];

        json newConifgParams;

        for (const auto& param : paramsData["Param"])
        {
            json materialData;

            bool existed = false;
            for (const auto& existedParam : config["Param"])
            {
                if (existedParam["Name"] == param["Name"])
                {
                    if (existedParam["Type"] == param["Type"])
                    {
                        materialData = existedParam;
                        existed = true;
                    }
                    break;
                }
            }

            if (existed)
            {
                json comparedSource = materialData;
                comparedSource.erase("Value");
                json comparedDestination = param;
                comparedDestination.erase("Default");
                if (comparedSource != comparedDestination)
                {
                    comparedSource = comparedDestination;
                    comparedSource["Value"] = materialData["Value"];
                    materialData = comparedSource;
                }
            }
            else
            {
                materialData["Name"] = param["Name"];
                materialData = param;
                materialData.erase("Default");

                auto defaultVal = param.find("Default");
                if (defaultVal != param.end())
                {
                    if (materialData["Type"] == ShaderParamType::SPT_Texture)
                    {
                        if (param["Default"] == "Black")
                        {
                            materialData["Value"] = "black.dds";
                        }
                        if (param["Default"] == "Gray")
                        {
                            materialData["Value"] = "gray.dds";
                        }
                        else if (param["Default"] == "White")
                        {
                            materialData["Value"] = "white.dds";
                        }
                        else if (param["Default"] == "Normal")
                        {
                            materialData["Value"] = "normal.dds";
                        }
                    }
                    else
                    {
                        materialData["Value"] = param["Default"];
                    }
                }
                else
                {
                    if (materialData["Type"] == ShaderParamType::SPT_Texture)
                    {
                        materialData["Value"] = "white.dds";
                    }
                    else if (materialData["Type"] == ShaderParamType::SPT_Float)
                    {
                        materialData["Value"] = 1.0;
                    }
                    else if (materialData["Type"] == ShaderParamType::SPT_Color || materialData["Type"] == ShaderParamType::SPT_Float4)
                    {
                        materialData["Value"] = { 1.0f,1.0f,1.0f,1.0f };
                    }
                }
            }

            newConifgParams.emplace_back(materialData);
        }

        config["Param"] = newConifgParams;

        // write prettified JSON to another file
        std::ofstream outFile(configFile);
        outFile << std::setw(4) << config << std::endl;
        outFile.close();

        configFileLastSaveTime = fs::last_write_time(configFile);
    }

    void Material::ReloadConfig()
    {
        std::wstring shaderName = Utility::ToWide(config["ShaderName"]);

        for (auto& shaderStage : config["Stage"])
        {
            //std::cout << shaderStage << std::endl;

            std::shared_ptr<Shader> shader = Shader::Resolve(shaderName, shaderStage);
            ShaderManager::GetInstance().AddToUsingPool(shader);

            shaders.emplace_back(shader);
        }

        DynamicConstantProcess::RawLayout DCBLayout;
        for (auto& param : config["Param"])
        {
            if (param["Type"] == ShaderParamType::SPT_Texture)
            {
                auto matExsists = textureMap.find(param["Name"]);

                if (matExsists != textureMap.end() && matExsists->second != nullptr)
                {
                    if (Utility::ToNarrow(matExsists->second->GetName()) != param["Value"])
                    {
                        SetTexture(param["Name"], Texture::Resolve(Utility::ToWide(param["Value"])), textureSlotMap[param["Name"]]);

                        //not a good idea, should be re-considered
                        std::shared_ptr<CommandQueue> commandQueue = Graphics::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
                        uint64_t fenceValue = Graphics::GetInstance().ExecuteCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
                        commandQueue->WaitForFenceValue(fenceValue);
                    }
                }
                else
                {
                    SetTexture(param["Name"], Texture::Resolve(Utility::ToWide(param["Value"])));
                }
            }
            else if(param["Type"] == ShaderParamType::SPT_Float)
            {
                DCBLayout.Add<DynamicConstantProcess::Float>(param["Name"]);
            }
            else if (param["Type"] == ShaderParamType::SPT_Float4 || param["Type"] == ShaderParamType::SPT_Color)
            {
                DCBLayout.Add<DynamicConstantProcess::Float4>(param["Name"]);
            }
            else if (param["Type"] == ShaderParamType::SPT_Bool)
            {
                DCBLayout.Add<DynamicConstantProcess::Bool>(param["Name"]);
            }
        }

        DynamicConstantProcess::Buffer DXBBuffer = DynamicConstantProcess::Buffer(std::move(DCBLayout));
        for (auto& param : config["Param"])
        {
            if (param["Type"] == ShaderParamType::SPT_Float)
            {
                DXBBuffer[param["Name"]] = param["Value"].get<float>();
            }
            else if (param["Type"] == ShaderParamType::SPT_Float4 || param["Type"] == ShaderParamType::SPT_Color)
            {
                dx::XMFLOAT4 float4 = { param["Value"][0],param["Value"][1],param["Value"][2],param["Value"][3] };
                DXBBuffer[param["Name"]] = float4;
            }
            else if (param["Type"] == ShaderParamType::SPT_Bool)
            {
                DXBBuffer[param["Name"]] = param["Value"].get<bool>();
            }
        }

        auto matExsists = dynamicConstantMap.find(Utility::ToNarrow(name));

        if (matExsists != dynamicConstantMap.end() && matExsists->second != nullptr)
        {
            dynamicConstantMap[Utility::ToNarrow(name)]->Update(DXBBuffer);
        }
        else
        {
            std::shared_ptr<DynamicConstantBufferInHeap> baseMat = std::make_shared<DynamicConstantBufferInHeap>(Utility::ToNarrow(name), DXBBuffer);
            SetConstant(Utility::ToNarrow(name), baseMat);
        }
    }

    std::vector<std::shared_ptr<BindableObject::Shader>> Material::GetShaders() const noexcept
    {
        return shaders;
    }

    bool Material::IsShaderDirty() noexcept
    {
        bool isShaderDirty = false;
        for (std::shared_ptr<Shader>& shader : shaders)
        {
            if (shader)
            {
                isShaderDirty |= shader->IsDirty();
            }
        }

        return isShaderDirty;
    }

    void Material::SetTexture(const std::string textureName, const std::shared_ptr<Texture> texture) noexcept
    {
        SetTexture(textureName, texture, numTextureIndices);
    }

    void Material::SetTexture(const std::string textureName, const std::shared_ptr<Texture> texture, UINT slot) noexcept
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
        textureMap[textureName] = texture;
        textureSlotMap[textureName] = slot;

        indexDirty = true;
    }

    void Material::SetConstant(const std::string constantName, const std::shared_ptr<DynamicBufferInHeap> constant) noexcept
    {
        SetConstant(constantName, constant, numConstantIndices);
    }

    void Material::SetConstant(const std::string constantName, const std::shared_ptr<DynamicBufferInHeap> constant, UINT slot) noexcept
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

        indexDirty = true;
    }

    void Material::Bind() noxnd
    {
        if (IsShaderDirty())
        {
            std::wstring shaderName = Utility::ToWide(config["ShaderName"]);

            UploadConfig(shaderName);
            ReloadConfig();
        }

        if (indexDirty)
        {
            indexConstantBuffer->Update(shaderResourceIndices.data(), shaderResourceIndices.size() * sizeof(UINT));
            indexDirty = false;
        }
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