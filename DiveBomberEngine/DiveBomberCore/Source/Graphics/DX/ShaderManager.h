#pragma once
#include "..\GraphicsHeader.h"

#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

namespace DiveBomber::DEResource
{
	class Shader;
	enum class ShaderType;
	class PipelineStateObject;
}

namespace DiveBomber::DX
{
	class ShaderManager final
	{
	public:
		ShaderManager();
		ShaderManager(const ShaderManager&) = delete;
		ShaderManager& operator =(const ShaderManager&) = delete;

		[[nodiscard]] wrl::ComPtr<ID3DBlob> Compile(const std::string& hlslFile, const std::wstring shaderDirectory, const std::wstring shaderName, DEResource::ShaderType shaderType);
		void AddToUsingPool(std::shared_ptr<DEResource::Shader> shader) noexcept;
		void ReCompileShader();
		void ResetAllShaderDirtyState() noexcept;
		void DeleteShaderInUsingPool(const std::string key) noexcept;

		[[nodiscard]] static ShaderManager& GetInstance();
		static void Destructor() noexcept;

	private:
		// Responsible for the actual compilation of shaders.
		wrl::ComPtr<IDxcCompiler3> compiler;

		// Used to create include handle and provides interfaces for loading shader to blob, etc.
		wrl::ComPtr<IDxcUtils> utils;
		wrl::ComPtr<IDxcIncludeHandler> includeHandler;

		std::unordered_map<std::string, std::shared_ptr<DEResource::Shader>> shaderPool;

		std::mutex shaderManagerMutex;

		static std::unique_ptr<ShaderManager> instance;
	};
}