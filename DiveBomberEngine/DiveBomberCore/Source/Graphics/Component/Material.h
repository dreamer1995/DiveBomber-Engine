#pragma once
#include "..\GraphicsHeader.h"

#include "..\..\Utility\DEJson.h"

#include <vector>
#include <unordered_map>

namespace DiveBomber::BindableObject
{
	class Texture;
	class Shader;
	template<typename C>
	class ConstantBuffer;
	class DynamicConstantBufferInHeap;
	class RenderTargetAsShaderResourceView;
	class BindableShaderInput;
}

namespace DiveBomber::Component
{
	using json = nlohmann::json;
	namespace fs = std::filesystem;

	enum class ShaderParamType
	{
		SPT_Float,
		SPT_Float4,
		SPT_Bool,
		SPT_Color,
		SPT_Texture,
	};

	class Material final
	{
	public:
		Material(const std::wstring inputName);
		void SetTexture(const std::shared_ptr<BindableObject::BindableShaderInput> texture) noexcept;
		void SetTexture(const std::shared_ptr<BindableObject::BindableShaderInput> texture, UINT slot) noexcept;
		void SetTexture(const std::string textureName, const std::shared_ptr<BindableObject::BindableShaderInput> texture) noexcept;
		void SetTexture(const std::string textureName, const std::shared_ptr<BindableObject::BindableShaderInput> texture, UINT slot) noexcept;

		void SetConstant(const std::shared_ptr<BindableObject::BindableShaderInput> constant) noexcept;
		void SetConstant(const std::shared_ptr<BindableObject::BindableShaderInput> constant, UINT slot) noexcept;
		void SetConstant(const std::string constantName, const std::shared_ptr<BindableObject::BindableShaderInput> constant) noexcept;
		void SetConstant(const std::string constantName, const std::shared_ptr<BindableObject::BindableShaderInput> constant, UINT slot) noexcept;

		void Bind() noxnd;

		[[nodiscard]] std::wstring GetName() const noexcept;

		void SetMaterialParameterScalar(std::string constantName, std::string key, float scalar) const noexcept;
		void SetMaterialParameterVector(std::string constantName, std::string key, DirectX::XMFLOAT4 vector) const noexcept;

		void GetConfig();
		void UploadConfig(const std::wstring shaderName);
		void ReloadConfig();

		[[nodiscard]] std::vector<std::shared_ptr<BindableObject::Shader>> GetShaders() const noexcept;
		[[nodiscard]] bool IsShaderDirty() noexcept;
	private:
		void CreateDefaultConfig();
		[[nodiscard]] int ParamTypeStringToEnum(std::string string) noexcept;
		[[nodiscard]] int ShaderStageStringToEnum(std::string string) noexcept;
		[[nodiscard]] int ParamTypeToDynamicConstantType(ShaderParamType type) noexcept;

		std::wstring name;
		std::shared_ptr<BindableObject::ConstantBuffer<UINT>> indexConstantBuffer;
		UINT numConstantIndices = 0;
		UINT numTextureIndices = 0;
		std::vector<UINT> shaderResourceIndices;
		bool indexDirty = false;
		std::unordered_map<std::string, std::shared_ptr<BindableObject::DynamicConstantBufferInHeap>> dynamicConstantMap;
		std::unordered_map<std::string, std::pair<std::shared_ptr<BindableObject::Texture>, UINT>> textureMap;

		json config;
		std::filesystem::path configFile;
		std::filesystem::file_time_type configFileLastSaveTime;

		std::vector<std::shared_ptr<BindableObject::Shader>> shaders;
	};
}