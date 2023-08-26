#include "ShaderManager.h"

#include "..\..\Exception\GraphicsException.h"
#include "..\BindableObject\Shader.h"

namespace DiveBomber::Component
{
    using namespace DEException;
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
}