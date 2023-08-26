#include "Shader.h"

#include "..\..\DiveBomberCore.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\..\Utility\GlobalParameters.h"

#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;

	Shader::Shader(Graphics& inputGfx, const std::wstring& inputName, ShaderType inputType)
		:
		gfx(inputGfx),
		name(inputName),
		type(inputType)
	{
		RecompileShader();
	}

	wrl::ComPtr<ID3DBlob> Shader::GetBytecode() const noexcept
	{
		return bytecodeBlob;
	}

	void Shader::RecompileShader()
	{
		//todo
		HRESULT hr;

		const std::wstring builtShaderPath(ProjectDirectoryW L"Asset\\Shader\\Built\\" + name + L".cso");
		GFX_THROW_INFO(D3DReadFileToBlob(builtShaderPath.c_str(), &bytecodeBlob));
	}

	void Shader::Bind(Graphics& gfx) noxnd
	{
		//HRESULT hr;
		//GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u));
	}

	std::shared_ptr<Shader> Shader::Resolve(Graphics& gfx, const std::wstring& name, ShaderType type)
	{
		return gfx.GetParent().ResolveBindable<Shader>(gfx, name, type);
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
}