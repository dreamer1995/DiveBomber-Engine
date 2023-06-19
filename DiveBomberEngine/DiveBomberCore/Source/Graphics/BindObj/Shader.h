#pragma once
#include "Bindable.h"
#include "..\Graphics.h"

namespace DiveBomber::BindObj
{
	class Shader final : public Bindable
	{
	public:
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
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		static std::shared_ptr<Shader> Resolve(DEGraphics::Graphics& gfx, const std::string& path, ShaderType type);
		static std::string GenerateUID(const std::string& path, ShaderType type);
		std::string GetUID() const noexcept override;
	private:
		DEGraphics::Graphics& gfx;
		std::string path;
		wrl::ComPtr<ID3DBlob> pBytecodeBlob;
		ShaderType type;
	};
}
