#include "Material.h"

#include "..\Graphics\GraphicResource\Shader.h"
#include "..\Graphics\GraphicResource\Bindable\ConstantBufferInRootSignature.h"
#include "..\Graphics\GraphicResource\ShaderInputable\DynamicConstantBufferInHeap.h"
#include "..\Graphics\GraphicResource\ShaderInputable\Texture.h"
#include "..\Graphics\DX\ShaderManager.h"
#include "..\Graphics\DX\CommandQueue.h"
#include "..\Graphics\DX\GlobalResourceManager.h"

#include <iostream>
#include <any>
#include <format>
#include <..\imgui\imgui.h>

namespace DiveBomber::DEComponent
{
    using namespace DEGraphics;
    using namespace GraphicResource;
    using namespace DX;

    Material::Material(const fs::path inputPath, const fs::path inputDefaultShaderPath)
        :
        Component(inputPath.stem()),
        ConfigDrivenResource(inputPath.wstring() + L".deasset"),
        shaderPath(inputDefaultShaderPath)
    {
        using namespace std::string_literals;
        indexConstantBuffer = std::make_shared<ConstantBufferInRootSignature<UINT>>(name + L"#"s + L"IndexConstant", 7u);

        RefreshMaterial();
    }

    void DiveBomber::DEComponent::Material::ApplyConfig()
    {
        fs::path configFileCachePath(ProjectDirectoryW L"Cache\\Material\\" + configFilePath.filename().wstring());
        ReadConfig(configFileCachePath);

        shaderPath = Utility::ToWide(config["ShaderPath"]);

        fs::file_time_type configFileLastSaveTime = fs::last_write_time(configFilePath);
        fs::file_time_type builtShaderLastSaveTime = fs::last_write_time(shaderPath);
        if (builtShaderLastSaveTime > configFileLastSaveTime)
        {
            CreateConfig();
        }
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
            return EngineTextureDirectory "black.dds";
        }
        if (defaultTexture == "Gray")
        {
            return EngineTextureDirectory "gray.dds";
        }
        else if (defaultTexture == "White")
        {
            return EngineTextureDirectory "white.dds";
        }
        else if (defaultTexture == "Normal")
        {
            return EngineTextureDirectory "normal.dds";
        }

        return "";
    }

    void Material::CreateConfig()
    {
        config["ConfigFileType"] = 0u;
        std::wstring paramString = Shader::GetShaderParamsString(shaderPath);

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
                    // saved value type should be same
                    // if type not match, set back to default.
                    if (existedParam["Type"] == param["Type"] &&
                        existedParam["Value"].type() == ParamTypeEnumToJsonTypeEnum(param["Type"]))
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
                    comparedSource["Value"] = materialData["Value"];
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
                    switch ((ShaderParamType)materialData["Type"])
                    {
                    case ShaderParamType::SPT_Texture:
                        materialData["Value"] = EngineTextureDirectory "white.dds";
                        break;
                    case ShaderParamType::SPT_Float:
                        materialData["Value"] = 1.0;
                        break;
                    case ShaderParamType::SPT_Color:
                    case ShaderParamType::SPT_Float4:
                        materialData["Value"] = { 1.0f,1.0f,1.0f,1.0f };
                        break;
                    case ShaderParamType::SPT_Bool:
                        materialData["Value"] = true;
                        break;
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
        std::ofstream outFile(configFilePath);
        outFile << std::setw(4) << config << std::endl;
        outFile.close();
    }

    void Material::LoadResourceFromConfig()
    {
        std::wstring shaderPath = Utility::ToWide(config["ShaderPath"]);
        shaders.clear();
        for (auto& shaderStage : config["Stage"])
        {
            //std::cout << shaderStage << std::endl;

            std::shared_ptr<Shader> shader = GlobalResourceManager::Resolve<Shader>(shaderPath, shaderStage);
            ShaderManager::GetInstance().AddToUsingPool(shader);

            shaders.emplace_back(shader);
        }

        DynamicConstantProcess::RawLayout DCBLayout;
        UINT textureCounter = 0;
        for (auto& param : config["Param"])
        {
            switch ((ShaderParamType)param["Type"])
            {
            case ShaderParamType::SPT_Texture:
            {
                auto matExsists = textureMap.find(param["Name"]);

                if (matExsists != textureMap.end() && matExsists->second.first != nullptr)
                {
                    if (Utility::ToNarrow(matExsists->second.first->GetName()) != param["Value"])
                    {
                        SetTexture(param["Name"].get<std::string>(), GlobalResourceManager::Resolve<Texture>(Utility::ToWide(param["Value"])), textureCounter);
                    }
                    else if (textureCounter != matExsists->second.second)
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
                break;
            }
            case ShaderParamType::SPT_Float:
                DCBLayout.Add<DynamicConstantProcess::Float>(param["Name"]);
                break;
            case ShaderParamType::SPT_Float4:
            case ShaderParamType::SPT_Color:
                DCBLayout.Add<DynamicConstantProcess::Float4>(param["Name"]);
                break;
            case ShaderParamType::SPT_Bool:
                DCBLayout.Add<DynamicConstantProcess::Bool>(param["Name"]);
                break;
            }
        }
        if (config["Param"].size() == 0)
        {
            DCBLayout.Add<DynamicConstantProcess::Integer>("PlaceHolder");
        }
        DynamicConstantProcess::Buffer DXBBuffer = DynamicConstantProcess::Buffer(std::move(DCBLayout));
        for (auto& param : config["Param"])
        {
            switch ((ShaderParamType)param["Type"])
            {
            case ShaderParamType::SPT_Float:
                DXBBuffer[param["Name"]] = param["Value"].get<float>();
                break;
            case ShaderParamType::SPT_Float4:
            case ShaderParamType::SPT_Color:
                dx::XMFLOAT4 float4 = { param["Value"][0],param["Value"][1],param["Value"][2],param["Value"][3] };
                DXBBuffer[param["Name"]] = float4;
                break;
            case ShaderParamType::SPT_Bool:
                DXBBuffer[param["Name"]] = param["Value"].get<bool>();
                break;
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

    void Material::SaveConfig()
    {
        auto buf = dynamicConstantMap[Utility::ToNarrow(name)]->GetBuffer();

        for (auto& param : config["Param"])
        {
            switch ((ShaderParamType)param["Type"])
            {
            case ShaderParamType::SPT_Float:
                param["Value"] = (float)buf[param["Name"]];
                break;
            case ShaderParamType::SPT_Float4:
            case ShaderParamType::SPT_Color:
                dx::XMFLOAT4 float4 = buf[param["Name"]];
                param["Value"] = { float4.x,float4.y,float4.z,float4.w };
                break;
            case ShaderParamType::SPT_Bool:
                param["Value"] = (bool)buf[param["Name"]];
                break;
            }
        }

        // write prettified JSON to another file
        std::ofstream outFile(configFilePath);
        outFile << std::setw(4) << config << std::endl;
        outFile.close();
    }

    void Material::RefreshMaterial()
    {
        ApplyConfig();
        LoadResourceFromConfig();
    }

    std::vector<std::shared_ptr<GraphicResource::Shader>> Material::GetShaders() const noexcept
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

    std::shared_ptr<GraphicResource::ConstantBufferInRootSignature<UINT>> Material::GetIndexConstantBuffer() const noexcept
    {
        return indexConstantBuffer;
    }

    void Material::DrawDetailPanel()
    {
        DrawComponentUI();
    }

    void Material::DrawComponentUI()
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::CollapsingHeader("Material"))
        {
            auto buf = dynamicConstantMap[Utility::ToNarrow(name)]->GetBuffer();

            using namespace std::string_literals;
            auto tag = [tagScratch = std::string{}, tagString = "##"s + Utility::ToNarrow(name)]
            (std::string label) mutable
            {
                tagScratch = label + tagString;
                return tagScratch.c_str();
            };

            for (auto& param : config["Param"])
            {
                float dirty = false;
                const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };

                switch ((ShaderParamType)param["Type"])
                {
                case ShaderParamType::SPT_Float:
                {
                    auto value = buf[param["Name"]];
                    dcheck(ImGui::SliderFloat(tag(param["Name"].get<std::string>()), &value,
                        param["Min"].get<float>(), param["Max"].get<float>(),
                        param["Format"].get<std::string>().c_str(),
                        param["PowerStep"].get<bool>() ? ImGuiSliderFlags_Logarithmic : ImGuiSliderFlags_None));
                    break;
                }
                case ShaderParamType::SPT_Float4:
                {
                    auto value = buf[param["Name"]];
                    dcheck(ImGui::SliderFloat4(tag(param["Name"].get<std::string>()), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT4&>(value)),
                        param["Min"].get<float>(), param["Max"].get<float>(),
                        param["Format"].get<std::string>().c_str(),
                        param["PowerStep"].get<bool>() ? ImGuiSliderFlags_Logarithmic : ImGuiSliderFlags_None));
                    break;
                }
                case ShaderParamType::SPT_Color:
                {
                    auto value = buf[param["Name"]];
                    dcheck(ImGui::ColorPicker4(tag(param["Name"].get<std::string>()), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT4&>(value))));
                    break;
                }
                case ShaderParamType::SPT_Bool:
                {
                    auto value = buf[param["Name"]];
                    dcheck(ImGui::Checkbox(tag(param["Name"].get<std::string>()), &value));
                    break;
                }
                case ShaderParamType::SPT_Texture:
                {
                    ImGui::Text(tag(param["Name"].get<std::string>()));

                    const ImVec2 borderSize = { 115.0f,115.0f };
                    ImVec2 iconSize = { 100.0f,100.0f };
                    ImGui::BeginChild(tag(param["Name"].get<std::string>()), borderSize, ImGuiChildFlags_Border);
                        std::shared_ptr<Texture> texture = GlobalResourceManager::GetInstance().Resolve<Texture>(
                            param["Value"], Texture::TextureLoadType::TLT_Icon);
                        const D3D12_RESOURCE_DESC texDesc = texture->GetTextureBuffer()->GetDesc();
                        const float XYRatio = texDesc.Width / (float)texDesc.Height;
                        if (XYRatio > 1)
                        {
                            iconSize.y /= XYRatio;
                        }
                        else
                        {
                            iconSize.x *= XYRatio;
                        }

                        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - iconSize.x) * 0.5f);
                        ImGui::SetCursorPosY((ImGui::GetWindowSize().y - iconSize.y) * 0.5f);
                        ImGui::Image((ImTextureID)texture->GetSRVDescriptorGPUHandle().ptr,
                            ImVec2(iconSize.x, iconSize.y));
                    ImGui::EndChild();
                    break;
                }
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
            RefreshMaterial();

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