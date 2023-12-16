#include "Texture.h"

#include "..\..\..\Utility\GlobalParameters.h"
#include "..\..\GraphicsSource.h"
#include "..\..\DX\GlobalResourceManager.h"
#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"
#include "..\..\DX\CommandList.h"
#include "..\..\DX\ResourceStateTracker.h"

#include <DirectXTex\DirectXTexEXR.h>
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"Iex-3_2_d.lib")
#pragma comment(lib,"IlmThread-3_2_d.lib")
#pragma comment(lib,"Imath-3_2_d.lib")
#pragma comment(lib,"OpenEXR-3_2_d.lib")
#pragma comment(lib,"OpenEXRCore-3_2_d.lib")
#pragma comment(lib,"OpenEXRUtil-3_2_d.lib")

namespace DiveBomber::DEResource
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;
	namespace fs = std::filesystem;
	namespace dx = DirectX;

	Texture::Texture(const std::wstring& inputName)
		:
		Texture(inputName, TextureDescription{})
	{
	}
	Texture::Texture(const std::wstring& inputName, TextureDescription inputTextureDesc)
		:
		Resource(inputName),
		textureDesc(inputTextureDesc)
	{
		LoadTexture();
	}

	Texture::~Texture()
	{
		ResourceStateTracker::RemoveGlobalResourceState(textureBuffer);
	}

	UINT Texture::GetSRVDescriptorHeapOffset() const noexcept
	{
		return descriptorAllocation->GetBaseOffset();
	}

	std::shared_ptr<Texture> Texture::Resolve(const std::wstring& name)
	{
		return GlobalResourceManager::Resolve<Texture>(name);
	}

	std::shared_ptr<Texture> Texture::Resolve(const std::wstring& name, TextureDescription textureDesc)
	{
		return GlobalResourceManager::Resolve<Texture>(name, textureDesc);
	}

	std::wstring Texture::GetName() const noexcept
	{
		return name;
	}

	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID(ProjectDirectoryW L"Asset\\Texture\\" + name, descriptorAllocation);
	}

	void Texture::LoadTexture()
	{
		fs::path cachePath(ProjectDirectoryW L"Cache\\Texture\\" + name);
		cachePath.replace_extension(".dds");
		if (fs::exists(cachePath))
		{
			LoadTextureFromCache(cachePath);
		}
		else
		{
			fs::path filePath(ProjectDirectoryW L"Asset\\Texture\\" + name);
			LoadTextureFromRaw(filePath);
		}
	}

	void Texture::LoadTextureFromCache(const fs::path& filePath)
	{
		dx::TexMetadata metadata;
		dx::ScratchImage scratchRawImage;

		HRESULT hr;

		GFX_THROW_INFO(LoadFromDDSFile(
			filePath.c_str(),
			dx::DDS_FLAGS_NONE,
			&metadata,
			scratchRawImage));

		LoadScratchImage(scratchRawImage);
	}

	void Texture::LoadTextureFromRaw(const fs::path& filePath)
	{
		if (!fs::exists(filePath))
		{
			throw std::exception("File not found.");
		}

		dx::TexMetadata metadata;
		dx::ScratchImage scratchRawImage;

		HRESULT hr;

		if (filePath.extension() == ".dds")
		{
			GFX_THROW_INFO(LoadFromDDSFile(
				filePath.c_str(),
				dx::DDS_FLAGS_NONE,
				&metadata,
				scratchRawImage));
		}
		else if (filePath.extension() == ".hdr")
		{
			GFX_THROW_INFO(LoadFromHDRFile(
				filePath.c_str(),
				&metadata,
				scratchRawImage));
		}
		else if (filePath.extension() == ".tga")
		{
			GFX_THROW_INFO(LoadFromTGAFile(
				filePath.c_str(),
				&metadata,
				scratchRawImage));
		}
		else if (filePath.extension() == ".exr")
		{
			GFX_THROW_INFO(LoadFromEXRFile(
				filePath.c_str(),
				&metadata,
				scratchRawImage));
		}
		else
		{
			GFX_THROW_INFO(LoadFromWICFile(
				filePath.c_str(),
				dx::WIC_FLAGS_NONE,
				&metadata,
				scratchRawImage));
		}

		dx::ScratchImage scratchImage;

		DXGI_FORMAT format = scratchRawImage.GetImages()->format;
		bool generrateMipNotSupport = dx::IsCompressed(format) || dx::IsTypeless(format) ||
			dx::IsPlanar(format) || dx::IsPalettized(format);

		if (textureDesc.generateMip && !generrateMipNotSupport)
		{
			GFX_THROW_INFO(dx::GenerateMipMaps(*scratchRawImage.GetImages(), dx::TEX_FILTER_LINEAR, 0, scratchImage));
		}
		else
		{
			scratchImage = std::move(scratchRawImage);
		}

		metadata.mipLevels = scratchImage.GetImageCount();

		fs::path cachePath(ProjectDirectoryW L"Cache\\Texture\\" + name);
		cachePath.replace_extension(".dds");
		dx::SaveToDDSFile(scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, dx::DDS_FLAGS_NONE, cachePath.c_str());

		LoadScratchImage(scratchImage);
	}

	void Texture::LoadScratchImage(const dx::ScratchImage& scratchImage)
	{
		const auto& chainBase = *scratchImage.GetImages();
		const D3D12_RESOURCE_DESC texDesc{
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Width = (UINT)chainBase.width,
			.Height = (UINT)chainBase.height,
			.DepthOrArraySize = 1,
			.MipLevels = textureDesc.generateMip ? (UINT16)scratchImage.GetImageCount() : 1u,
			.Format = chainBase.format,
			.SampleDesc = {.Count = 1 },
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = D3D12_RESOURCE_FLAG_NONE,
		};

		HRESULT hr;

		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
		GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&textureBuffer)
		));

		ResourceStateTracker::AddGlobalResourceState(textureBuffer, D3D12_RESOURCE_STATE_COMMON);

		std::vector<D3D12_SUBRESOURCE_DATA> subresourceData;

		for (int i = 0; i < texDesc.MipLevels; i++)
		{
			const auto img = scratchImage.GetImage(i, 0, 0);

			auto subresourceClip = D3D12_SUBRESOURCE_DATA{
				.pData = img->pixels,
				.RowPitch = (LONG_PTR)img->rowPitch,
				.SlicePitch = (LONG_PTR)img->slicePitch,
			};

			subresourceData.emplace_back(subresourceClip);
		}

		{
			wrl::ComPtr<ID3D12Resource> textureUploadBuffer;

			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
			const auto uploadBufferSize = GetRequiredIntermediateSize(
				textureBuffer.Get(), 0, (UINT)subresourceData.size()
			);
			const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
			GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&textureUploadBuffer)
			));

			std::shared_ptr<CommandList> copyCommandList = Graphics::GetInstance().GetCommandList(D3D12_COMMAND_LIST_TYPE_COPY);

			UpdateSubresources(
				copyCommandList->GetGraphicsCommandList().Get(),
				textureBuffer.Get(),
				textureUploadBuffer.Get(),
				0, 0,
				(UINT)subresourceData.size(),
				subresourceData.data()
			);

			copyCommandList->TrackResource(textureUploadBuffer);
		}

		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(textureBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, true);

		const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{
				.Format = texDesc.Format,
				.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Texture2D{.MipLevels = texDesc.MipLevels },
		};

		descriptorAllocation = Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate(1u);
		Graphics::GetInstance().GetDevice()->CreateShaderResourceView(textureBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());
	}
}
