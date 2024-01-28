#include "Material.h"

#include "..\Graphics\Resource\Shader.h"
#include "..\Graphics\Resource\Bindable\ConstantBufferInRootSignature.h"
#include "..\Graphics\Resource\ShaderInputable\DynamicConstantBufferInHeap.h"
#include "..\Graphics\Resource\ShaderInputable\Texture.h"
#include "..\Utility\GlobalParameters.h"
#include "..\Graphics\DX\ShaderManager.h"
#include "..\Graphics\DX\CommandQueue.h"
#include "..\Graphics\DX\GlobalResourceManager.h"

#include <iostream>
#include <fstream>
#include <any>
#include <..\imgui\imgui.h>

namespace DiveBomber::DEComponent
{
    using namespace DEGraphics;
    using namespace DEResource;
    using namespace DX;

    Material::Material(const std::wstring inputName, const std::wstring inputDefaultShaderName)
        :
        name(inputName),
        defaultShaderName(inputDefaultShaderName)
    {
        using namespace std::string_literals;
        indexConstantBuffer = std::make_shared<ConstantBufferInRootSignature<UINT>>(name + L"#"s + L"IndexConstant", 7u);

        configFile = ProjectDirectoryW L"Asset\\Material\\" + name + L".json";

        GetConfig();
        ReloadConfig();
    }

    void DiveBomber::DEComponent::Material::GetConfig()
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
        config["ShaderName"] = Utility::ToNarrow(defaultShaderName);

        UploadConfig(defaultShaderName);
    }

    int Material::ParamTypeStringToEnum(std::string string) const noexcept
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
        else if (string == "Bool")
        {
            return (int)ShaderParamType::SPT_Bool;
        }
        else
        {
            std::cout << "Unknow parameter type " + string + " in shader!" << std::endl;
            return -1;
        }
    }

    int Material::ShaderStageStringToEnum(std::string string) const noexcept
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

    int Material::ParamTypeToDynamicConstantType(ShaderParamType type) const noexcept
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

    json::value_t Material::ParamTypeEnumToJsonTypeEnum(ShaderParamType type) const noexcept
    {
        switch (type)
        {
        case ShaderParamType::SPT_Float:
            return json::value_t::number_float;
        case ShaderParamType::SPT_Color:
        case ShaderParamType::SPT_Float4:
            return json::value_t::array;
        case ShaderParamType::SPT_Bool:
            return json::value_t::boolean;
        case ShaderParamType::SPT_Texture:
            return json::value_t::string;
        default:
            std::cout << "Unknow shader param type: " + std::to_string((int)type) + "!" << std::endl;
            return json::value_t::null;
        }
    }

    std::string Material::TextureDefaultToTexturePath(std::string defaultTexture) const noexcept
    {
        if (defaultTexture == "Black")
        {
            return "black.dds";
        }
        if (defaultTexture == "Gray")
        {
            return "gray.dds";
        }
        else if (defaultTexture == "White")
        {
            return "white.dds";
        }
        else if (defaultTexture == "Normal")
        {
            return "normal.dds";
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

            // check if already set default, for exsist value resolve.
            bool hasDefault = param.find("Default") != param.end();

            // check if already exist in cache.
            bool existed = false;
            for (const auto& existedParam : config["Param"])
            {
                if (existedParam["Name"] == param["Name"])
                {
                    if (existedParam["Type"] == param["Type"] && hasDefault)
                    {
                        materialData = existedParam;
                        existed = true;
                    }
                    break;
                }
            }

            // if exist, just keep cache value, change rest elements.
            if (existed)
            {
                json comparedSource = materialData;
                comparedSource.erase("Value");
                json comparedDestination = param;
                comparedDestination.erase("Default");
                if (comparedSource != comparedDestination)
                {
                    comparedSource = comparedDestination;
                    // if type not match, set back to default.
                    if (ParamTypeEnumToJsonTypeEnum(param["Type"]) != materialData["Value"].type())
                    {
                        if (param["Type"] != ShaderParamType::SPT_Texture)
                        {
                            comparedSource["Value"] = param["Default"];
                        }
                        else
                        {
                            materialData["Value"] = TextureDefaultToTexturePath(materialData["Default"]);
                        }
                    }
                    else
                    {
                        comparedSource["Value"] = materialData["Value"];
                    }
                    materialData = comparedSource;
                }
            }
            // if not exist, create a new element, set appropriate default value.
            else
            {
                materialData["Name"] = param["Name"];
                materialData = param;

                if (hasDefault)
                {
                    materialData.erase("Default");

                    if (materialData["Type"] == ShaderParamType::SPT_Texture)
                    {
                        materialData["Value"] = TextureDefaultToTexturePath(materialData["Default"]);
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
                    else if (materialData["Type"] == ShaderParamType::SPT_Bool)
                    {
                        materialData["Value"] = true;
                    }
                }
            }

            // for min/max,power in float case.
            if (materialData["Type"] == ShaderParamType::SPT_Float ||
                materialData["Type"] == ShaderParamType::SPT_Float4)
            {
                auto minExsist = materialData.find("Min");
                if (minExsist == materialData.end())
                {
                    materialData["Min"] = -10000.0f;
                }

                auto maxExsist = materialData.find("Max");
                if (maxExsist == materialData.end())
                {
                    materialData["Max"] = 10000.0f;
                }

                auto powerExsist = materialData.find("PowerStep");
                if (powerExsist == materialData.end())
                {
                    materialData["PowerStep"] = false;
                }

                auto formatExsist = materialData.find("Format");
                if (formatExsist == materialData.end())
                {
                    materialData["Format"] = "%.2f";
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

            std::shared_ptr<Shader> shader = GlobalResourceManager::Resolve<Shader>(shaderName, shaderStage);
            ShaderManager::GetInstance().AddToUsingPool(shader);

            shaders.emplace_back(shader);
        }

        DynamicConstantProcess::RawLayout DCBLayout;
        UINT textureCounter = 0;
        for (auto& param : config["Param"])
        {
            if (param["Type"] == ShaderParamType::SPT_Texture)
            {
                auto matExsists = textureMap.find(param["Name"]);

                if (matExsists != textureMap.end() && matExsists->second.first != nullptr)
                {
                    if (Utility::ToNarrow(matExsists->second.first->GetName()) != param["Value"])
                    {
                        SetTexture(param["Name"].get<std::string>(), GlobalResourceManager::Resolve<Texture>(Utility::ToWide(param["Value"])), textureCounter);
                    }
                    else if(textureCounter != matExsists->second.second)
                    {
                        matExsists->second.second = textureCounter;
                        SetTexture(param["Name"].get<std::string>(), GlobalResourceManager::Resolve<Texture>(Utility::ToWide(param["Value"])), textureCounter);
                    }
                }
                else
                {
                    SetTexture(param["Name"], GlobalResourceManager::Resolve<Texture>(Utility::ToWide(param["Value"])), textureCounter);
                }
                textureCounter++;
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
        if (config["Param"].size() == 0)
        {
            DCBLayout.Add<DynamicConstantProcess::Integer>("PlaceHolder");
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
            std::shared_ptr<DynamicConstantBufferInHeap> baseMat = std::make_shared<DynamicConstantBufferInHeap>(name, DXBBuffer);
            SetConstant(Utility::ToNarrow(name), baseMat);
        }
    }

    std::vector<std::shared_ptr<DEResource::Shader>> Material::GetShaders() const noexcept
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

    std::shared_ptr<DEResource::ConstantBufferInRootSignature<UINT>> Material::GetIndexConstantBuffer() const noexcept
    {
        return indexConstantBuffer;
    }

    void Material::DrawComponentUI()
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::CollapsingHeader("Material"))
        {
            auto buf = dynamicConstantMap[Utility::ToNarrow(name)]->GetBuffer();

            for (auto& param : config["Param"])
            {
                float dirty = false;
                const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };

                if (param["Type"] == ShaderParamType::SPT_Float)
                {
                    auto value = buf[param["Name"]];
                    dcheck(ImGui::SliderFloat(param["Name"].get<std::string>().c_str(), &value,
                        param["Min"].get<float>(), param["Max"].get<float>(),
                        param["Format"].get<std::string>().c_str(),
                        param["PowerStep"].get<bool>() ? ImGuiSliderFlags_Logarithmic : ImGuiSliderFlags_None));
                }
                else if (param["Type"] == ShaderParamType::SPT_Float4)
                {
                    auto value = buf[param["Name"]];
                    dcheck(ImGui::SliderFloat4(param["Name"].get<std::string>().c_str(), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT4&>(value)),
                        param["Min"].get<float>(), param["Max"].get<float>(),
                        param["Format"].get<std::string>().c_str(),
                        param["PowerStep"].get<bool>() ? ImGuiSliderFlags_Logarithmic : ImGuiSliderFlags_None));
                }
                else if (param["Type"] == ShaderParamType::SPT_Color)
                {
                    auto value = buf[param["Name"]];
                    dcheck(ImGui::ColorPicker4(param["Name"].get<std::string>().c_str(), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT4&>(value))));
                }
                else if (param["Type"] == ShaderParamType::SPT_Bool)
                {
                    auto value = buf[param["Name"]];
                    dcheck(ImGui::Checkbox(param["Name"].get<std::string>().c_str(), &value));
                }
                else if (param["Type"] == ShaderParamType::SPT_Texture)
                {
                    // todo
                    ImGui::Text(param["Name"].get<std::string>().c_str());
                }

                if (dirty)
                {
                    dynamicConstantMap[Utility::ToNarrow(name)]->Update(buf);
                }
            }
        }
    }

    void Material::SetTexture(const std::string textureName, const std::shared_ptr<ShaderInputable> texture) noexcept
    {
        SetTexture(textureName, texture, numTextureIndices);
    }

    void Material::SetTexture(const std::string textureName, const std::shared_ptr<ShaderInputable> texture, UINT slot) noexcept
    {
        textureMap[textureName] = { std::dynamic_pointer_cast<Texture>(texture),slot };
        SetTexture(texture, slot);
    }

    void Material::SetTexture(const std::shared_ptr<ShaderInputable> texture) noexcept
    {
        SetTexture(texture, numTextureIndices);
    }

    void Material::SetTexture(const std::shared_ptr<ShaderInputable> texture, UINT slot) noexcept
    {
        SetTexture(texture->GetSRVDescriptorHeapOffset(), slot);
    }

    void Material::SetTexture(UINT srvDescriptorHeapOffset, UINT slot) noexcept
    {
        if (slot >= numTextureIndices)
        {
            UINT needInsert = slot - numTextureIndices + 1;
            std::vector<UINT>::iterator it = shaderResourceIndices.end();
            shaderResourceIndices.insert(it, needInsert, 0u);
            numTextureIndices = slot + 1;
        }

        UINT index = (numConstantIndices + slot);
        shaderResourceIndices[index] = srvDescriptorHeapOffset;

        indexDirty = true;
    }

    void Material::SetConstant(const std::string constantName, const std::shared_ptr<ShaderInputable> constant) noexcept
    {
        SetConstant(constantName, constant, numConstantIndices);
    }

    void Material::SetConstant(const std::string constantName, const std::shared_ptr<ShaderInputable> constant, UINT slot) noexcept
    {
        dynamicConstantMap[constantName] = std::dynamic_pointer_cast<DynamicConstantBufferInHeap>(constant);
        SetConstant(constant, slot);
    }

    void Material::SetConstant(const std::shared_ptr<ShaderInputable> constant) noexcept
    {
        SetConstant(constant, numConstantIndices);
    }

    void Material::SetConstant(const std::shared_ptr<ShaderInputable> constant, UINT slot) noexcept
    {
        SetConstant(constant->GetSRVDescriptorHeapOffset(), slot);
    }

    void Material::SetConstant(UINT srvDescriptorHeapOffse, UINT slot) noexcept
    {
        if (slot >= numConstantIndices)
        {
            UINT needInsert = slot - numConstantIndices + 1;
            std::vector<UINT>::iterator it = shaderResourceIndices.begin();
            shaderResourceIndices.insert(it + slot, needInsert, 0u);
            numConstantIndices = slot + 1;
        }
        shaderResourceIndices[slot] = srvDescriptorHeapOffse;

        indexDirty = true;
    }

    void Material::Bind() noxnd
    {
        if (IsShaderDirty())
        {
            std::wstring shaderName = Utility::ToWide(config["ShaderName"]);

            GetConfig();
            ReloadConfig();

            //not a good idea, should be re-considered
            std::shared_ptr<CommandQueue> commandQueue = Graphics::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
            uint64_t fenceValue = Graphics::GetInstance().ExecuteCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
            commandQueue->WaitForFenceValue(fenceValue);
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