#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::BindableObject
{
	enum class ShaderType
	{
		VertexShader,
		HullShader,
		DomainShader,
		GeometryShader,
		PixelShader,
		ComputeShader,
	};

	class Shader final : public Bindable
	{
	public:
		Shader(DEGraphics::Graphics& inputGfx, const std::wstring& inputName, ShaderType inputType);

		[[nodiscard]] wrl::ComPtr<ID3DBlob> GetBytecode() const noexcept;
		void RecompileShader();
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		[[nodiscard]] static std::shared_ptr<Shader> Resolve(DEGraphics::Graphics& gfx, const std::wstring& name, ShaderType type);
		[[nodiscard]] static std::string GenerateUID(const std::wstring& name, ShaderType type);
		[[nodiscard]] std::string GetUID() const noexcept override;
		[[nodiscard]] bool IsDirty() const noexcept;
		void SetDirty(bool inputIsDirty) noexcept;

	private:
		void LoadShaderBlob();

	private:
		DEGraphics::Graphics& gfx;
		std::wstring name;
		std::wstring directory;
		wrl::ComPtr<ID3DBlob> bytecodeBlob;
		ShaderType type;
		bool isDirty = false;
	};
}
