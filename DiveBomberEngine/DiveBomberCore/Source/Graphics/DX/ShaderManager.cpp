#include "ShaderManager.h"

#include "..\..\Exception\GraphicsException.h"
#include "..\BindableObject\Shader.h"
#include "..\..\Utility\GlobalParameters.h"

#include <iostream>
#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

namespace DiveBomber::DX
{
    using namespace DEException;
    using namespace BindableObject;

    ShaderManager::ShaderManager()
    {
        HRESULT hr;
        if (!utils)
        {
            GFX_THROW_INFO(::DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils)));
            GFX_THROW_INFO(::DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));
            GFX_THROW_INFO(utils->CreateDefaultIncludeHandler(&includeHandler));
        }
    }

    ShaderManager& ShaderManager::GetInstance() noexcept
    {
        static ShaderManager instance;
        return instance;
    }

    wrl::ComPtr<ID3DBlob> ShaderManager::Compile(const std::wstring shaderDirectory, const std::wstring shaderName,
        const std::wstring_view entryPoint, BindableObject::ShaderType shaderType)
    {
        return GetInstance().Compile_(shaderDirectory, shaderName, entryPoint, shaderType);
    }

    wrl::ComPtr<ID3DBlob> ShaderManager::Compile_(const std::wstring shaderDirectory, const std::wstring shaderName,
        const std::wstring_view entryPoint, BindableObject::ShaderType shaderType)
    {
        // Setup compilation arguments.
        const std::wstring targetProfile = [=]() 
        {
            switch (shaderType)
            {
            case ShaderType::VertexShader:
                return L"vs_6_6";
            case ShaderType::HullShader:
                return L"hs_6_6";
            case ShaderType::DomainShader:
                return L"ds_6_6";
            case ShaderType::GeometryShader:
                return L"gs_6_6";
            case ShaderType::PixelShader:
                return L"ps_6_6";
            case ShaderType::ComputeShader:
                return L"cs_6_6";
            default: {
                return L"";
            }
        }
        }();

        HRESULT hr;

        std::vector<LPCWSTR> compilationArguments;
        compilationArguments.emplace_back(L"-HV");
        compilationArguments.emplace_back(L"2021");

        compilationArguments.emplace_back(L"-E");
        compilationArguments.emplace_back(entryPoint.data());

        compilationArguments.emplace_back(L"-T");
        compilationArguments.emplace_back(targetProfile.c_str());

        compilationArguments.emplace_back(L"-I");
        compilationArguments.emplace_back(shaderDirectory.c_str());

        compilationArguments.emplace_back(L"-Fo");
        const std::wstring builtShaderPath(ProjectDirectoryW L"Asset\\Shader\\Built\\" + shaderName + L".cso");
        compilationArguments.emplace_back(builtShaderPath.c_str());

        compilationArguments.emplace_back(L"-Qstrip_debug");
        compilationArguments.emplace_back(L"-Qstrip_reflect");

        compilationArguments.emplace_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
        compilationArguments.emplace_back(DXC_ARG_DEBUG); //-Zi
        compilationArguments.emplace_back(DXC_ARG_ALL_RESOURCES_BOUND); //-all_resources_bound

        // Load the shader source file to a blob.
        wrl::ComPtr<IDxcBlobEncoding> sourceBlob{ nullptr };
        const std::wstring sourceFilePath(shaderDirectory + shaderName + L".hlsl");
        GFX_THROW_INFO(utils->LoadFile(sourceFilePath.c_str(), nullptr, &sourceBlob));

        const DxcBuffer sourceBuffer = {
            .Ptr = sourceBlob->GetBufferPointer(),
            .Size = sourceBlob->GetBufferSize(),
            .Encoding = 0u,
        };

        // Compile the shader.
        wrl::ComPtr<IDxcResult> compiledShaderBuffer{};
        GFX_THROW_INFO(compiler->Compile(&sourceBuffer, compilationArguments.data(),
            static_cast<uint32_t>(compilationArguments.size()), includeHandler.Get(),
            IID_PPV_ARGS(&compiledShaderBuffer)));

        wrl::ComPtr<IDxcBlobUtf8> errors{};
        GFX_THROW_INFO(compiledShaderBuffer->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr));

        wrl::ComPtr<ID3DBlob> bytecodeBlob;
        if (errors && errors->GetStringLength() > 0)
        {
            const LPCSTR errorMessage = errors->GetStringPointer();
            std::cout << errorMessage;
        }
        else
        {
            compiledShaderBuffer->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&bytecodeBlob), nullptr);
            D3DWriteBlobToFile(bytecodeBlob.Get(), builtShaderPath.c_str(), TRUE);
        }

        return bytecodeBlob;
    }

    void ShaderManager::AddPool(BindableObject::Shader* shader) noexcept
    {
        GetInstance().AddPool_(shader);
    }

    void ShaderManager::ReCompileShader()
    {
        GetInstance().ReCompileShader_();
    }

    void ShaderManager::ClearPool() noexcept
    {
        GetInstance().ClearPool_();
    }

    void ShaderManager::AddPool_(BindableObject::Shader* shader) noexcept
    {
        std::lock_guard<std::mutex> lock(shaderManagerMutex);
        shaderPool.emplace(shader->GetUID(), shader);
    }

    void ShaderManager::ReCompileShader_()
    {
        for (auto& shader : shaderPool)
        {
            shader.second->RecompileShader();
        }
    }

    void ShaderManager::ClearPool_() noexcept
    {
        shaderPool.clear();
    }
}