#include "Shader.h"

#include "..\..\DiveBomberCore.h"
#include "..\Graphics.h"
#include "GlobalBindableManager.h"
#include "GlobalBindableManager.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\..\Utility\GlobalParameters.h"
#include "..\DX\ShaderManager.h"

#include <filesystem>
#include <iostream>
#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;
	namespace fs = std::filesystem;

	Shader::Shader(Graphics& inputGfx, const std::wstring& inputName, ShaderType inputType)
		:
		gfx(inputGfx),
		name(inputName),
		type(inputType)
	{
		fs::path filePath = EngineDirectoryW L"Shader\\" + name + L".hlsl";
		if (fs::exists(filePath))
		{
			directory = EngineDirectoryW L"Shader\\";
		}
		filePath = ProjectDirectoryW L"Asset\\Shader\\" + name + L".hlsl";
		if (fs::exists(filePath))
		{
			directory = ProjectDirectoryW L"Asset\\Shader\\";
		}

		LoadShaderBlob();

		isDirty = false;
	}

	wrl::ComPtr<ID3DBlob> Shader::GetBytecode() const noexcept
	{
		return bytecodeBlob;
	}

	void Shader::RecompileShader()
	{
		wrl::ComPtr<ID3DBlob> compiledBlob = gfx.GetParent().GetShaderManager()->Compile(directory, name, L"main", type);
		if (compiledBlob)
		{
			bytecodeBlob = compiledBlob;
			isDirty = true;
			std::wcout << L"Recompile Shader: " + name << std::endl;
		}
	}

	void Shader::LoadShaderBlob()
	{
		const std::wstring builtShaderPath(ProjectDirectoryW L"Asset\\Shader\\Built\\" + name + L".cso");
		fs::path filePath = builtShaderPath;

		bool needRECompile = true;
		if (fs::exists(filePath))
		{
			needRECompile = false;
		}

		if (needRECompile)
		{
			RecompileShader();
		}
		else
		{
			HRESULT hr;
			GFX_THROW_INFO(D3DReadFileToBlob(builtShaderPath.c_str(), &bytecodeBlob));
		}
	}

	void Shader::Bind(Graphics& gfx) noxnd
	{

	}

	std::shared_ptr<Shader> Shader::Resolve(Graphics& gfx, const std::wstring& name, ShaderType type)
	{
		return gfx.GetParent().GetGlobalBindableManager()->Resolve<Shader>(gfx, name, type);
	}

	std::string Shader::GenerateUID(const std::wstring& name, ShaderType type)
	{
		using namespace std::string_literals;
		return typeid(Shader).name() + "#"s + Utility::ToNarrow(ProjectDirectoryW L"Asset\\Shader\\" + name) + "#"s + std::to_string((int)type);
	}

	std::string Shader::GetUID() const noexcept
	{
		return GenerateUID(name, type);
	}

	bool Shader::IsDirty() const noexcept
	{
		return isDirty;
	}

	void Shader::SetDirty(bool inputIsDirty) noexcept
	{
		isDirty = inputIsDirty;
	}
}