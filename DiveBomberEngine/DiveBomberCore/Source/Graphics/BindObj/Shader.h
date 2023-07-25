#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

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
		Shader(DEGraphics::Graphics& inputGfx, const std::wstring& inputPath, ShaderType inputType);
		[[nodiscard]] wrl::ComPtr<ID3DBlob> GetBytecode() const noexcept;
		void RecompileShader();
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		static std::shared_ptr<Shader> Resolve(DEGraphics::Graphics& gfx, const std::wstring& path, ShaderType type);
		static std::string GenerateUID(const std::wstring& path, ShaderType type);
		std::string GetUID() const noexcept override;
	private:
		DEGraphics::Graphics& gfx;
		std::wstring path;
		wrl::ComPtr<ID3DBlob> bytecodeBlob;
		ShaderType type;
	};
}
