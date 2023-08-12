#include "Shader.h"

#include "BindableCodex.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"

#include <d3dcompiler.h>
#pragma comment(lib,"D3DCompiler.lib")

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	Shader::Shader(Graphics& inputGfx, const std::wstring& inputPath, ShaderType inputType)
		:
		gfx(inputGfx),
		path(WOutputDirectory + inputPath),
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

		GFX_THROW_INFO(D3DReadFileToBlob(/*"ShaderBins\\" + */path.c_str(), &bytecodeBlob));
	}

	void Shader::Bind(Graphics& gfx) noxnd
	{
		//HRESULT hr;
		//GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u));
	}

	std::shared_ptr<Shader> Shader::Resolve(Graphics& gfx, const std::wstring& path, ShaderType type)
	{
		return Codex::Resolve<Shader>(gfx, path, type);
	}

	std::string Shader::GenerateUID(const std::wstring& path, ShaderType type)
	{
		using namespace std::string_literals;
		return typeid(Shader).name() + "#"s + Utility::ToNarrow(path) + "#"s + std::to_string((int)type);
	}

	std::string Shader::GetUID() const noexcept
	{
		return GenerateUID(path, type);
	}
}