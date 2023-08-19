#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::BindableObject
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
		Shader(DEGraphics::Graphics& inputGfx, const std::wstring& inputName, ShaderType inputType);
		[[nodiscard]] wrl::ComPtr<ID3DBlob> GetBytecode() const noexcept;
		void RecompileShader();
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		static std::shared_ptr<Shader> Resolve(DEGraphics::Graphics& gfx, const std::wstring& name, ShaderType type);
		static std::string GenerateUID(const std::wstring& name, ShaderType type);
		std::string GetUID() const noexcept override;
	private:
		DEGraphics::Graphics& gfx;
		std::wstring name;
		wrl::ComPtr<ID3DBlob> bytecodeBlob;
		ShaderType type;
	};
}
