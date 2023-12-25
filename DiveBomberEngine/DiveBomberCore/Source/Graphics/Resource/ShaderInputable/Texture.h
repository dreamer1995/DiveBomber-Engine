#pragma once
#include "..\Resource.h"
#include "ShaderInputable.h"
#include "..\..\GraphicsHeader.h"

#include <filesystem>
#include <..\DirectXTex\DirectXTex\DirectXTex.h>

namespace DiveBomber::DX
{
	class DescriptorAllocation;
}

namespace DiveBomber::DEResource
{
	class Texture final : public Resource, public ShaderInputable
	{
	public:
		struct TextureDescription
		{
			bool sRGB = false;
			bool generateMip = true;
		};

		struct TextureMipMapGenerateConstant
		{
			uint32_t srcMipLevel;           // Texture level of source mip
			uint32_t numMipLevels;          // Number of OutMips to write: [1-4]
			uint32_t srcDimension;          // Width and height of the source texture are even or odd.
			uint32_t isSRGB;                // Must apply gamma correction to sRGB textures.
			dx::XMFLOAT2 texelSize;			// 1.0 / OutMip1.Dimensions
		};

	public:
		Texture(const std::wstring& inputName);
		Texture(const std::wstring& inputName, TextureDescription inputTextureDesc);
		~Texture();

		void ChangeTexture(const std::wstring& inputName);
		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept override;
		
		[[nodiscard]] static std::shared_ptr<Texture> Resolve(const std::wstring& name);
		[[nodiscard]] static std::shared_ptr<Texture> Resolve(const std::wstring& name,
			TextureDescription textureDesc);
		std::wstring GetName() const noexcept;
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::wstring& name, Ignore&&...ignore)
		{
			using namespace std::string_literals;
			return typeid(Texture).name() + "#"s + Utility::ToNarrow(name);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;

	private:
		void LoadTexture();
		void LoadTextureFromCache(const std::filesystem::path& filePath);
		void LoadTextureFromRaw(const std::filesystem::path& filePath);
		void GenerateCache(const dx::ScratchImage& scratchImage);
		void LoadScratchImage(const dx::ScratchImage& scratchImage);
		void GenerateMipMaps();

	private:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		wrl::ComPtr<ID3D12Resource> textureBuffer;
		TextureDescription textureDesc;
	};
}