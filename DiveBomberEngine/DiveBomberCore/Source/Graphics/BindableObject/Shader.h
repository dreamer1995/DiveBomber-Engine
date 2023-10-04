#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

#include <filesystem>
#include <unordered_map>

namespace DiveBomber::Component
{
	class Material;
}

namespace DiveBomber::BindableObject
{
	namespace fs = std::filesystem;

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
		Shader(const std::wstring inputName, ShaderType inputType);

		[[nodiscard]] wrl::ComPtr<ID3DBlob> GetBytecode() const noexcept;
		void Bind() noxnd override;
		[[nodiscard]] static std::shared_ptr<Shader> Resolve(const std::wstring name, ShaderType type);
		[[nodiscard]] static std::string GenerateUID(const std::wstring name, ShaderType type);
		[[nodiscard]] std::string GetUID() const noexcept override;

		[[nodiscard]] bool IsDirty() const noexcept;
		void SetDirty(bool inputIsDirty) noexcept;

		void RecompileShader();
		[[nodiscard]] static std::wstring GetShaderParamsString(const std::wstring name);

		void AddMaterialReference(std::shared_ptr<Component::Material> material);
		void AddMaterialReference(const std::wstring key);

		[[nodiscard]] fs::file_time_type GetSourceFileLastSaveTime() const noexcept;
		[[nodiscard]] static fs::file_time_type GetSourceFileLastSaveTime(const std::wstring name) noexcept;

	private:
		[[nodiscard]] fs::path FindSourceFilePath() noexcept;
		[[nodiscard]] static fs::path FindSourceFilePath(const std::wstring name) noexcept;
		[[nodiscard]] std::wstring GetShaderTypeAbbreviation() const noexcept;
		void LoadShader();

	private:
		std::wstring name;
		std::wstring directory;
		fs::path sourceFile;
		fs::file_time_type sourceLastSaveTime;
		fs::path builtFile;
		fs::file_time_type builtLastSaveTime;
		wrl::ComPtr<ID3DBlob> bytecodeBlob;
		ShaderType type;
		bool isDirty = false;

		std::unordered_map<std::wstring, std::shared_ptr<Component::Material>> materialMap;
	};
}
