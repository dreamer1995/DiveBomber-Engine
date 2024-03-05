#pragma once
#include "..\..\Resource.h"
#include "..\GraphicsHeader.h"

#include <filesystem>
#include <unordered_map>

namespace DiveBomber::DEComponent
{
	class Material;
}

namespace DiveBomber::GraphicResource
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

	class Shader final : public DiveBomber::Resource
	{
	public:
		Shader(const fs::path inputPath, ShaderType inputType);

		[[nodiscard]] wrl::ComPtr<ID3DBlob> GetBytecode() const noexcept;
		[[nodiscard]] static std::string GenerateUID(const std::wstring name, ShaderType type);
		[[nodiscard]] std::string GetUID() const noexcept override;

		[[nodiscard]] bool IsDirty() const noexcept;
		void SetDirty(bool inputIsDirty) noexcept;

		[[nodiscard]] ShaderType GetShaderType() const noexcept;

		void LoadShader();
		void RecompileShader();
		[[nodiscard]] static std::wstring GetShaderParamsString(const fs::path path);

		void AddMaterialReference(std::shared_ptr<DEComponent::Material> material);
		void AddMaterialReference(const std::wstring key);

	private:
		[[nodiscard]] std::wstring GetShaderTypeAbbreviation() const noexcept;

	private:
		std::wstring directory;
		fs::path sourceFile;
		fs::file_time_type sourceLastSaveTime;
		fs::path builtFile;
		fs::file_time_type builtLastSaveTime;
		wrl::ComPtr<ID3DBlob> bytecodeBlob;
		ShaderType type;
		bool isDirty = false;

		std::unordered_map<std::wstring, std::shared_ptr<DEComponent::Material>> materialMap;
	};
}
