#pragma once
#include "..\Resource.h"
#include "ShaderInputable.h"
#include "..\..\GraphicsHeader.h"
#include "..\..\..\Utility\DEJson.h"

#include <filesystem>
#include <..\DirectXTex\DirectXTex\DirectXTex.h>

namespace DiveBomber::DX
{
	class DescriptorAllocation;
}

namespace DiveBomber::DEResource
{
	namespace fs = std::filesystem;
	using json = nlohmann::json;

	class Texture : public Resource, public ShaderInputable
	{
	public:
		enum class TextureDimension
		{
			TDS_Texture1D = 2,
			TDS_Texture1D_Array = 3,
			TDS_Texture2D = 4,
			TDS_Texture2D_Array = 5,
			TDS_Texture3D = 8,
			TDS_TextureCube = 9,
			TDS_TextureCube_Array = 10,
		};

		struct TextureParam
		{
			bool sRGB = false;
			bool generateMip = false;
			TextureDimension textureDimension = TextureDimension::TDS_Texture2D;
		};

		enum class TextureLoadType
		{
			TLT_Standard,
			TLT_Icon,
			TLT_DiffuseIrradiance,
			TLT_SpecularMip
		};

		struct alignas(16) TextureMipMapGenerateConstant
		{
			uint32_t srcMipLevel;           // Texture level of source mip
			uint32_t numMipLevels;          // Number of OutMips to write: [1-4]
			uint32_t srcDimension;          // Width and height of the source texture are even or odd.
			uint32_t isSRGB;                // Must apply gamma correction to sRGB textures.
			dx::XMFLOAT2 texelSize;			// 1.0 / OutMip1.Dimensions
		};

		struct alignas(16) TextureDiffuseMipGenerateConstant
		{
			alignas(16) uint32_t isSRGB;    // Must apply gamma correction to sRGB textures.
			dx::XMFLOAT2 texelSize;			// 1.0 / OutMip1.Dimensions
		};

		struct alignas(16) TextureSpecularMipGenerateConstant
		{
			float roughness;				// Simulated roughness
			uint32_t isSRGB;                // Must apply gamma correction to sRGB textures.
			dx::XMFLOAT2 texelSize;			// 1.0 / OutMip1.Dimensions
		};

		struct alignas(16) TextureCubeMapGenerateConstant
		{
			alignas(16) uint32_t readSRGB;	// Must apply gamma correction to sRGB textures.
			dx::XMFLOAT2 texelSize;			// 1.0 / OutMip1.Dimensions
		};

	public:
		Texture(const fs::path& inputPath, const TextureLoadType inputTextureLoadType = TextureLoadType::TLT_Standard);
		~Texture();

		void ReloadTexture();
		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRVDescriptorCPUHandle() const noexcept;
		[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetSRVDescriptorGPUHandle() const noexcept;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetTextureBuffer() const noexcept;
		static void UpdateConfig(const fs::path& outputPath, const TextureParam inputTextureParam);
		
		std::wstring GetName() const noexcept;
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const fs::path& inputPath, const TextureLoadType textureLoadType = TextureLoadType::TLT_Standard, Ignore&&...ignore)
		{
			using namespace std::string_literals;
			return typeid(Texture).name() + "#"s + inputPath.string() + std::to_string((UINT)textureLoadType);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;

	protected:
		void GetConfig();
		void LoadTexture();
		void GenerateCache(const wrl::ComPtr<ID3D12Resource>& outputTextureBuffer, const std::filesystem::path& outputPath, bool cubemap = false);
		void GenerateCache(const dx::Image* images, size_t numImages, const dx::TexMetadata texMetaData, const std::filesystem::path& outputPath);
		void GenerateMipMaps(wrl::ComPtr<ID3D12Resource>& uavBuffer);
		void GenerateDiffuseIrradiance(wrl::ComPtr<ID3D12Resource>& uavBuffer, const std::filesystem::path& outputPath);
		void GenerateSpecularIBLMipMaps(wrl::ComPtr<ID3D12Resource>& uavBuffer, const std::filesystem::path& outputPath);
		void GenerateCubeMap(wrl::ComPtr<ID3D12Resource>& uavBuffer, wrl::ComPtr<ID3D12Resource>& cubeSourceTextureBuffer, bool readSRGB);
		void GenerateIconMap(wrl::ComPtr<ID3D12Resource>& uavBuffer, const std::filesystem::path& outputPath);
		[[nodiscard]] DXGI_FORMAT GetUAVCompatableFormat(DXGI_FORMAT format);
		[[nodiscard]] bool CheckSRGBFormat(DXGI_FORMAT format);
		[[nodiscard]] D3D12_RESOURCE_DIMENSION SRVDimensionToResourceDimension(TextureDimension textureDimension) noxnd;

	protected:
		json config;
		fs::path filePath;
		fs::path cachePath;
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		wrl::ComPtr<ID3D12Resource> textureBuffer;
		TextureParam textureParam;
		TextureLoadType textureLoadType;
	};
}