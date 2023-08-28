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
}

namespace DiveBomber::DX
{
	class ShaderManager final
	{
	public:
		ShaderManager();
		[[nodiscard]] static ShaderManager& GetInstance() noexcept;
		[[nodiscard]] static wrl::ComPtr<ID3DBlob> Compile(const std::wstring shaderDirectory, const std::wstring shaderName,
			const std::wstring_view entryPoint, BindableObject::ShaderType shaderType);

		static void AddPool(BindableObject::Shader* shader) noexcept;
		static void ReCompileShader();
		static void ClearPool() noexcept;

	private:
		wrl::ComPtr<ID3DBlob> Compile_(const std::wstring shaderDirectory, const std::wstring shaderName,
			const std::wstring_view entryPoint, BindableObject::ShaderType shaderType);
		void AddPool_(BindableObject::Shader* shader) noexcept;
		void ReCompileShader_();
		void ClearPool_() noexcept;

	private:
		// Responsible for the actual compilation of shaders.
		wrl::ComPtr<IDxcCompiler3> compiler;

		// Used to create include handle and provides interfaces for loading shader to blob, etc.
		wrl::ComPtr<IDxcUtils> utils;
		wrl::ComPtr<IDxcIncludeHandler> includeHandler;

		std::unordered_map<std::string, BindableObject::Shader*> shaderPool;

		std::mutex shaderManagerMutex;
	};
}