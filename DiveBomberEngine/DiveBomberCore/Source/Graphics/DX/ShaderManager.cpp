#include "ShaderManager.h"

#include "..\..\Exception\GraphicsException.h"
#include "..\..\Utility\GlobalParameters.h"
#include "..\BindableObject\Shader.h"
#include "..\BindableObject\PipelineStateObject.h"

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

    wrl::ComPtr<ID3DBlob> ShaderManager::Compile(const std::wstring shaderDirectory, const std::wstring shaderName,
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
        wrl::ComPtr<IDxcBlobUtf16> debugDataPath{};
        GFX_THROW_INFO(compiledShaderBuffer->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), &debugDataPath));

        wrl::ComPtr<ID3DBlob> bytecodeBlob;
        if (errors && errors->GetStringLength() > 0)
        {
            const LPCSTR errorMessage = errors->GetStringPointer();
            std::cout << errorMessage;
        }
        else
        {
            compiledShaderBuffer->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&bytecodeBlob), &debugDataPath);
            D3DWriteBlobToFile(bytecodeBlob.Get(), builtShaderPath.c_str(), TRUE);
        }

        return bytecodeBlob;
    }

    void ShaderManager::AddToUsingPool(std::shared_ptr<BindableObject::Shader> shader) noexcept
    {
        std::lock_guard<std::mutex> lock(shaderManagerMutex);
        shaderPool.emplace(shader->GetUID(), shader);
    }

    void ShaderManager::AddToUsingPool(std::shared_ptr<BindableObject::PipelineStateObject> PSO) noexcept
    {
        std::lock_guard<std::mutex> lock(shaderManagerMutex);
        pipelineStateObjectPool.emplace(PSO->GetUID(), PSO);
    }

    void ShaderManager::ReCompileShader()
    {
        {
            auto it = shaderPool.begin();
            while (it != shaderPool.end())
            {
                if (it->second)
                {
                    it->second->RecompileShader();
                    it++;
                }
                else
                {
                    shaderPool.erase(it);
                }
            }
        }
        
        {
            auto it = pipelineStateObjectPool.begin();
            while (it != pipelineStateObjectPool.end())
            {
                if (it->second)
                {
                    if (it->second->IsShaderDirty())
                    {
                        it->second->UpdatePipelineState();
                    }
                    it++;
                }
                else
                {
                    pipelineStateObjectPool.erase(it);
                }
            }
        }
        
        {
            auto it = shaderPool.begin();
            while (it != shaderPool.end())
            {
                if (it->second)
                {
                    it->second->SetDirty(false);
                    it++;
                }
                else
                {
                    shaderPool.erase(it);
                }
            }
        }
    }

    void ShaderManager::DeleteShaderInUsingPool(const std::string key) noexcept
    {
        auto it = shaderPool.find(key);
        if (it != shaderPool.end())
        {
            shaderPool.erase(it);
        }
    }

    void ShaderManager::DeletePipelineStateObjectInUsingPool(const std::string key) noexcept
    {
        auto it = pipelineStateObjectPool.find(key);
        if (it != pipelineStateObjectPool.end())
        {
            pipelineStateObjectPool.erase(it);
        }
    }
}