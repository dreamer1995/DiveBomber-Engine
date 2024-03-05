#pragma once
#include "Component.h"
#include "..\Graphics\GraphicsHeader.h"
#include "..\Utility\GlobalParameters.h"
#include "..\Utility\DEJson.h"
#include "..\Graphics\UI\DetailModifier.h"

#include <vector>
#include <unordered_map>

namespace DiveBomber::GraphicResource
{
	class Texture;
	class Shader;
	template<typename C>
	class ConstantBufferInRootSignature;
	class DynamicConstantBufferInHeap;
	class RenderTargetAsShaderResourceView;
	class ShaderInputable;
}

namespace DiveBomber::DEComponent
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

	class Material final : public Component, public UI::DetailModifier
	{
	public:
		Material(const fs::path inputPath, const fs::path inputDefaultShaderPath = EngineShaderDirectoryW L"Default");
		void SetTexture(const std::shared_ptr<GraphicResource::ShaderInputable> texture) noexcept;
		void SetTexture(const std::shared_ptr<GraphicResource::ShaderInputable> texture, UINT slot) noexcept;
		void SetTexture(const std::string textureName, const std::shared_ptr<GraphicResource::ShaderInputable> texture) noexcept;
		void SetTexture(const std::string textureName, const std::shared_ptr<GraphicResource::ShaderInputable> texture, UINT slot) noexcept;
		void SetTexture(const UINT srvDescriptorHeapOffset, UINT slot) noexcept;

		void SetConstant(const std::shared_ptr<GraphicResource::ShaderInputable> constant) noexcept;
		void SetConstant(const std::shared_ptr<GraphicResource::ShaderInputable> constant, UINT slot) noexcept;
		void SetConstant(const std::string constantName, const std::shared_ptr<GraphicResource::ShaderInputable> constant) noexcept;
		void SetConstant(const std::string constantName, const std::shared_ptr<GraphicResource::ShaderInputable> constant, UINT slot) noexcept;
		void SetConstant(const UINT srvDescriptorHeapOffset, UINT slot) noexcept;

		void Bind() noxnd;

		[[nodiscard]] std::wstring GetName() const noexcept;

		void SetMaterialParameterScalar(std::string constantName, std::string key, float scalar) const noexcept;
		void SetMaterialParameterVector(std::string constantName, std::string key, DirectX::XMFLOAT4 vector) const noexcept;

		void GetConfig();
		void UploadConfig(const fs::path shaderPath);
		void ReloadConfig();

		[[nodiscard]] std::vector<std::shared_ptr<GraphicResource::Shader>> GetShaders() const noexcept;
		[[nodiscard]] bool IsShaderDirty() noexcept;

		[[nodiscard]] std::shared_ptr<GraphicResource::ConstantBufferInRootSignature<UINT>> GetIndexConstantBuffer() const noexcept;

		void DrawDetailPanel() override;
		void DrawComponentUI() override;

		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const fs::path& name, Ignore&&...ignore)
		{
			using namespace std::string_literals;
			return typeid(Resource).name() + "#"s + name.string();
		}
	private:
		void GetConfigFromRaw();
		void CreateDefaultConfig();
		[[nodiscard]] int ParamTypeStringToEnum(std::string string) const noexcept;
		[[nodiscard]] int ShaderStageStringToEnum(std::string string) const noexcept;
		[[nodiscard]] int ParamTypeToDynamicConstantType(ShaderParamType type) const noexcept;
		[[nodiscard]] json::value_t ParamTypeEnumToJsonTypeEnum(ShaderParamType type) const noexcept;
		[[nodiscard]] std::string TextureDefaultToTexturePath(std::string) const noexcept;

		std::shared_ptr<GraphicResource::ConstantBufferInRootSignature<UINT>> indexConstantBuffer;
		UINT numConstantIndices = 0;
		UINT numTextureIndices = 0;
		std::vector<UINT> shaderResourceIndices;
		bool indexDirty = false;
		std::unordered_map<std::string, std::shared_ptr<GraphicResource::DynamicConstantBufferInHeap>> dynamicConstantMap;
		std::unordered_map<std::string, std::pair<std::shared_ptr<GraphicResource::Texture>, UINT>> textureMap;

		json config;
		fs::path configFilePath;

		std::vector<std::shared_ptr<GraphicResource::Shader>> shaders;

		fs::path defaultShaderPath;
	};
}