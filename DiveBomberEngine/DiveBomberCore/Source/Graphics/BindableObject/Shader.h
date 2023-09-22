#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

#include <filesystem>

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
		Shader(const std::wstring& inputName, ShaderType inputType);

		[[nodiscard]] wrl::ComPtr<ID3DBlob> GetBytecode() const noexcept;
		[[nodiscard]] std::wstring RecompileShader();
		void Bind() noxnd override;
		[[nodiscard]] static std::shared_ptr<Shader> Resolve(const std::wstring& name, ShaderType type);
		[[nodiscard]] static std::string GenerateUID(const std::wstring& name, ShaderType type);
		[[nodiscard]] std::string GetUID() const noexcept override;
		[[nodiscard]] bool IsDirty() const noexcept;
		void SetDirty(bool inputIsDirty) noexcept;
		[[nodiscard]] std::wstring GetShaderTypeAbbreviation() const noexcept;
		[[nodiscard]] std::wstring LoadShaderBlob();

	private:
		std::wstring name;
		std::wstring directory;
		std::filesystem::path sourceFile;
		std::filesystem::file_time_type sourceLastSaveTime;
		std::filesystem::path builtFile;
		std::filesystem::file_time_type builtLastSaveTime;
		wrl::ComPtr<ID3DBlob> bytecodeBlob;
		ShaderType type;
		bool isDirty = false;
	};
}
