#include "ShaderManager.h"

#include "..\..\Exception\GraphicsException.h"
#include "..\..\Utility\GlobalParameters.h"
#include "..\Resource\Shader.h"
#include "..\Resource\Bindable\PipelineStateObject.h"

#include <iostream>
#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

namespace DiveBomber::DX
{
    using namespace DEException;
    using namespace DEResource;
    namespace fs = std::filesystem;

    std::unique_ptr<ShaderManager> ShaderManager::instance;

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

    wrl::ComPtr<ID3DBlob> ShaderManager::Compile(const std::string& hlslFile, const std::wstring shaderDirectory, const std::wstring shaderName, DEResource::ShaderType shaderType)
    {
        // Setup compilation arguments.
        const std::wstring shaderTypeAbbreviation = [=]()
            {
                switch (shaderType)
                {
                case ShaderType::VertexShader:
                    return L"VS";
                case ShaderType::HullShader:
                    return L"HS";
                case ShaderType::DomainShader:
                    return L"DS";
                case ShaderType::GeometryShader:
                    return L"GS";
                case ShaderType::PixelShader:
                    return L"PS";
                case ShaderType::ComputeShader:
                    return L"CS";
                default: {
                    return L"";
                }
                }
            }();

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

        const std::wstring entryPoint = shaderTypeAbbreviation + L"Main";

        HRESULT hr;

        std::vector<LPCWSTR> compilationArguments;
        compilationArguments.emplace_back(L"-HV");
        compilationArguments.emplace_back(L"2021");

        compilationArguments.emplace_back(L"-E");
        compilationArguments.emplace_back(entryPoint.c_str());

        compilationArguments.emplace_back(L"-T");
        compilationArguments.emplace_back(targetProfile.c_str());

        compilationArguments.emplace_back(L"-I");
        compilationArguments.emplace_back(shaderDirectory.c_str());

#ifdef _DEBUG
        //compilationArguments.emplace_back(L"-Fo");
        //const std::wstring builtShaderBinPath(shaderName + L".cso");
        //compilationArguments.emplace_back(builtShaderBinPath.c_str());

        //compilationArguments.emplace_back(L"-Fd");
        //const std::wstring builtShaderPDBPath(shaderName + L".pdb");
        //compilationArguments.emplace_back(builtShaderPDBPath.c_str());
        compilationArguments.emplace_back(L"-Qembed_debug");
        compilationArguments.emplace_back(DXC_ARG_DEBUG); //-Zi
        compilationArguments.emplace_back(DXC_ARG_SKIP_OPTIMIZATIONS); //-Od
#else
        compilationArguments.emplace_back(L"-Qstrip_reflect");
        compilationArguments.push_back(L"-Qstrip_debug");
        compilationArguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif // _DEBUG

        compilationArguments.emplace_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
        //compilationArguments.emplace_back(DXC_ARG_ALL_RESOURCES_BOUND); //-all_resources_bound

        // Load the shader source file to a blob.
        wrl::ComPtr<IDxcBlobEncoding> sourceBlob;
        GFX_THROW_INFO(utils->CreateBlob(hlslFile.c_str(), (UINT)hlslFile.size(), DXC_CP_ACP, &sourceBlob));

        const DxcBuffer sourceBuffer = {
            .Ptr = sourceBlob->GetBufferPointer(),
            .Size = sourceBlob->GetBufferSize(),
            .Encoding = DXC_CP_ACP,
        };

        // Compile the shader.
        wrl::ComPtr<IDxcResult> compiledShaderBuffer{};
        GFX_THROW_INFO(compiler->Compile(&sourceBuffer, compilationArguments.data(),
            static_cast<uint32_t>(compilationArguments.size()), includeHandler.Get(),
            IID_PPV_ARGS(&compiledShaderBuffer)));

        wrl::ComPtr<IDxcBlobUtf8> errors{};
        wrl::ComPtr<IDxcBlobUtf16> dataPath{};
        GFX_THROW_INFO(compiledShaderBuffer->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), &dataPath));

        wrl::ComPtr<ID3DBlob> bytecodeBlob;
        if (errors && errors->GetStringLength() > 0)
        {
            const LPCSTR errorMessage = errors->GetStringPointer();
            std::cout << errorMessage;
        }
        else
        {
            compiledShaderBuffer->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&bytecodeBlob), &dataPath);

            const fs::path builtShaderDirectory(ProjectDirectoryW L"Cache\\BuiltShader\\");
            if (!fs::exists(builtShaderDirectory))
            {
                fs::create_directories(builtShaderDirectory);
            }

            const std::wstring builtShaderPath(builtShaderDirectory.c_str() + shaderName + shaderTypeAbbreviation + L".cso");
            D3DWriteBlobToFile(bytecodeBlob.Get(), builtShaderPath.c_str(), TRUE);
        }

        return bytecodeBlob;
    }

    void ShaderManager::AddToUsingPool(std::shared_ptr<DEResource::Shader> shader) noexcept
    {
        std::lock_guard<std::mutex> lock(shaderManagerMutex);
        shaderPool[shader->GetUID()] = shader;
    }

    void ShaderManager::ReLoadShader()
    {
        auto it = shaderPool.begin();
        while (it != shaderPool.end())
        {
            if (it->second)
            {
                it->second->LoadShader();
                it++;
            }
            else
            {
                shaderPool.erase(it);
            }
        }
    }

    void ShaderManager::ResetAllShaderDirtyState() noexcept
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

    void ShaderManager::DeleteShaderInUsingPool(const std::string key) noexcept
    {
        auto it = shaderPool.find(key);
        if (it != shaderPool.end())
        {
            shaderPool.erase(it);
        }
    }

    ShaderManager& ShaderManager::GetInstance()
    {
        if (instance == nullptr)
        {
            instance = std::make_unique<ShaderManager>();
        }
        return *instance;
    }

    void ShaderManager::Destructor() noexcept
    {
        if (instance != nullptr)
        {
            instance.reset();
        }
    }
}