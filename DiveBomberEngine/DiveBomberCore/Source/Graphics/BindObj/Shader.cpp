#include "Shader.h"
#include "BindableCodex.h"

namespace DiveBomber::BindObj
{
	using namespace DEException;
	Shader::Shader(DEGraphics::Graphics& inputGfx, const std::string& inputPath, ShaderType inputType)
		:
		gfx(inputGfx)
	{
		path = inputPath;
		type = inputType;
		RecompileShader();
	}

	ID3DBlob* Shader::GetBytecode() const noexcept
	{
		return pBytecodeBlob.Get();
	}

	bool Shader::RecompileShader()
	{
		//todo
		HRESULT hr;
		GFX_THROW_INFO(D3DReadFileToBlob(Utility::ToWide(/*"ShaderBins\\" + */path).c_str(), &pBytecodeBlob));
	}
}