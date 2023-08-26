#pragma once
#include "..\GraphicsHeader.h"

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

namespace DiveBomber::BindableObject
{
	class Shader;
	enum class ShaderType;
}

namespace DiveBomber::Component
{
	class ShaderManager final
	{
	public:
		ShaderManager();
		[[nodiscard]] static ShaderManager& GetInstance() noexcept;
		void Compile(BindableObject::ShaderType shaderType);

	private:
		// Responsible for the actual compilation of shaders.
		wrl::ComPtr<IDxcCompiler3> compiler;

		// Used to create include handle and provides interfaces for loading shader to blob, etc.
		wrl::ComPtr<IDxcUtils> utils;
		wrl::ComPtr<IDxcIncludeHandler> includeHandler;
	};
}