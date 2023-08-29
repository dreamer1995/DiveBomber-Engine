#pragma once
#include "..\GraphicsHeader.h"

#include <string>
#include <unordered_map>
#include <mutex>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

namespace DiveBomber::BindableObject
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

		wrl::ComPtr<ID3DBlob> Compile(const std::wstring shaderDirectory, const std::wstring shaderName,
			const std::wstring_view entryPoint, BindableObject::ShaderType shaderType);
		void AddToUsingPool(std::shared_ptr<BindableObject::Shader> shader) noexcept;
		void AddToUsingPool(std::shared_ptr<BindableObject::PipelineStateObject> PSO) noexcept;
		void ReCompileShader();
		void DeleteShaderInUsingPool(const std::string key) noexcept;
		void DeletePipelineStateObjectInUsingPool(const std::string key) noexcept;

	private:
		// Responsible for the actual compilation of shaders.
		wrl::ComPtr<IDxcCompiler3> compiler;

		// Used to create include handle and provides interfaces for loading shader to blob, etc.
		wrl::ComPtr<IDxcUtils> utils;
		wrl::ComPtr<IDxcIncludeHandler> includeHandler;

		std::unordered_map<std::string, std::shared_ptr<BindableObject::Shader>> shaderPool;
		std::unordered_map<std::string, std::shared_ptr<BindableObject::PipelineStateObject>> pipelineStateObjectPool;

		std::mutex shaderManagerMutex;
	};
}