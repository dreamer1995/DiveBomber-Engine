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
		struct TextureParam
		{
			bool sRGB = false;
			bool generateMip = true;
			bool cubeMap = false;
			bool globalIllumination = false;
			bool textureArray = false;
			bool texture3D = false;
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
		Texture(const std::wstring& inputName, TextureParam inputTextureDesc = TextureParam{});
		~Texture();

		void ChangeTexture(const std::wstring& inputName);
		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRVDescriptorCPUHandle() const noexcept;
		[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetSRVDescriptorGPUHandle() const noexcept;
		[[nodiscard]] wrl::ComPtr<ID3D12Resource> GetTextureBuffer() const noexcept;
		
		std::wstring GetName() const noexcept;
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::wstring& name, Ignore&&...ignore)
		{
			using namespace std::string_literals;
			return typeid(Texture).name() + "#"s + Utility::ToNarrow(name);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;

	protected:
		void GetConfig();
		void UpdateConfig(const TextureParam inputTextureParam);
		void LoadTexture();
		void GenerateCache(const wrl::ComPtr<ID3D12Resource> outputTextureBuffer, const std::filesystem::path& outputPath);
		void LoadScratchImage(const std::filesystem::path& filePath);
		void GenerateMipMaps(wrl::ComPtr<ID3D12Resource>& uavBuffer);
		void GenerateDiffuseIrradiance(wrl::ComPtr<ID3D12Resource>& uavBuffer, const std::filesystem::path& outputPath);
		void GenerateSpecularIBLMipMaps(wrl::ComPtr<ID3D12Resource>& uavBuffer, const std::filesystem::path& outputPath);
		void GenerateCubeMap(wrl::ComPtr<ID3D12Resource>& uavBuffer, bool readSRGB);
		[[nodiscard]] DXGI_FORMAT GetUAVCompatableFormat(DXGI_FORMAT format);
		[[nodiscard]] bool CheckSRGBFormat(DXGI_FORMAT format);

	protected:
		json config;
		fs::path configFilePath;
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		wrl::ComPtr<ID3D12Resource> textureBuffer;
		TextureParam textureParam;
	};
}