#pragma once
#include "Bindable.h"
#include "..\Graphics.h"

namespace DiveBomber::BindObj
{
	class Shader final : public Bindable
	{
		enum class ShaderType
		{
			VertexShader,
			HullShader,
			DomainShader,
			GeometryShader,
			PixelShader,
			ComputeShder,
		};
	public:
		Shader(DEGraphics::Graphics& inputGfx, const std::string& inputPath, ShaderType inputType);
		wrl::ComPtr<ID3DBlob> GetBytecode() const noexcept;
		bool RecompileShader();
	private:
		DEGraphics::Graphics& gfx;
		std::string path;
		wrl::ComPtr<ID3DBlob> pBytecodeBlob;
		ShaderType type;
	};
}
