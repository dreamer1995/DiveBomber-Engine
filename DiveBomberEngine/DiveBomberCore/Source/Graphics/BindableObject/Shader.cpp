#include "Shader.h"

#include "..\..\DiveBomberCore.h"
#include "..\Graphics.h"
#include "GlobalBindableManager.h"
#include "GlobalBindableManager.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\..\Utility\GlobalParameters.h"
#include "..\DX\ShaderManager.h"
#include "..\Component\Material.h"

#include <print>
#include <iostream>
#include <fstream>
#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;
	namespace fs = std::filesystem;

	Shader::Shader(const std::wstring inputName, ShaderType inputType)
		:
		name(inputName),
		type(inputType)
	{
		builtFile = ProjectDirectoryW L"Asset\\Shader\\Built\\" + name + GetShaderTypeAbbreviation() + L".cso";

		sourceFile = FindSourceFilePath();
		directory = sourceFile.parent_path();

		LoadShader();
	}

	wrl::ComPtr<ID3DBlob> Shader::GetBytecode() const noexcept
	{
		return bytecodeBlob;
	}

	void Shader::RecompileShader()
	{
		std::wstring line;
		std::string hlslFile;

		std::wifstream rawFile;
		rawFile.open(sourceFile);

		while (std::getline(rawFile, line)) {
			if (line != L"\"Properties\"")
			{
				hlslFile.append(Utility::ToNarrow(line));
				hlslFile.append("\n");
			}
			else
			{
				std::getline(rawFile, line);
				while (line != L"\"/Properties\"")
				{
					//paramsFile.append(line);
					//paramsFile.append(L"\n");
					std::getline(rawFile, line);
				}
			}
		}

		rawFile.close();

		wrl::ComPtr<ID3DBlob> compiledBlob = ShaderManager::GetInstance().Compile(hlslFile, directory, name, type);
		if (compiledBlob)
		{
			bytecodeBlob = compiledBlob;
			isDirty = true;
			std::wcout << L"Recompile Shader: " + name + GetShaderTypeAbbreviation() << std::endl;
		}
	}

	std::wstring Shader::GetShaderParamsString(const std::wstring name)
	{
		fs::path sourceFilePath = FindSourceFilePath(name);

		std::wstring line;
		std::wstring paramsFile;

		std::wifstream rawFile;
		rawFile.open(sourceFilePath);

		while (std::getline(rawFile, line)) {
			if (line == L"\"Properties\"")
			{
				std::getline(rawFile, line);
				while (line != L"\"/Properties\"")
				{
					paramsFile.append(line);
					paramsFile.append(L"\n");
					std::getline(rawFile, line);
				}
				break;
			}
		}

		rawFile.close();

		return paramsFile;
	}

	void Shader::LoadShader()
	{
		bool needRecompile = true;
		if (fs::exists(builtFile))
		{
			sourceLastSaveTime = fs::last_write_time(sourceFile);
			builtLastSaveTime = fs::last_write_time(builtFile);
			if (builtLastSaveTime > sourceLastSaveTime)
			{
				needRecompile = false;
			}
		}

		if (needRecompile)
		{
			RecompileShader();
		}

		if (!isDirty)
		{
			HRESULT hr;
			GFX_THROW_INFO(D3DReadFileToBlob(builtFile.c_str(), &bytecodeBlob));
		}

		isDirty = false;
	}

	void Shader::AddMaterialReference(std::shared_ptr<Component::Material> material)
	{
		materialMap.emplace(material->GetName(), material);
	}

	void Shader::AddMaterialReference(const std::wstring key)
	{
	}

	fs::file_time_type Shader::GetSourceFileLastSaveTime() const noexcept
	{
		return sourceLastSaveTime;
	}

	fs::file_time_type Shader::GetSourceFileLastSaveTime(const std::wstring name) noexcept
	{
		std::wstring sourceFilePath = FindSourceFilePath(name);
		return fs::last_write_time(sourceFilePath);
	}

	fs::path Shader::FindSourceFilePath() noexcept
	{
		return FindSourceFilePath(name);
	}

	fs::path Shader::FindSourceFilePath(const std::wstring name) noexcept
	{
		fs::path sourceFilePath;
		fs::path filePath = EngineDirectoryW L"Shader\\" + name + L".hlsl";
		if (fs::exists(filePath))
		{
			sourceFilePath = filePath;
		}
		filePath = ProjectDirectoryW L"Asset\\Shader\\" + name + L".hlsl";
		if (fs::exists(filePath))
		{
			sourceFilePath = filePath;
		}

		return sourceFilePath;
	}

	void Shader::Bind() noxnd
	{

	}

	std::shared_ptr<Shader> Shader::Resolve(const std::wstring name, ShaderType type)
	{
		return GlobalBindableManager::Resolve<Shader>(name, type);
	}

	std::string Shader::GenerateUID(const std::wstring name, ShaderType type)
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

	ShaderType Shader::GetShaderType() const noexcept
	{
		return type;
	}

	std::wstring Shader::GetShaderTypeAbbreviation() const noexcept
	{
		switch (type)
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
	}
}