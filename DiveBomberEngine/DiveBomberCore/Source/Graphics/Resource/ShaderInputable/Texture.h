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
	class Texture final: public Resource, public ShaderInputable
	{
	public:
		struct TextureDescription
		{
			bool sRGB = false;
			bool generateMip = true;
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
		void GenerateCache(const dx::ScratchImage& scratchImage, dx::TexMetadata& metadata);
		void LoadScratchImage(const dx::ScratchImage& scratchImage);
		void GenerateMipMaps(const dx::ScratchImage& scratchRawImage, dx::ScratchImage& scratchImage);

	private:
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		wrl::ComPtr<ID3D12Resource> textureBuffer;
		TextureDescription textureDesc;
	};
}