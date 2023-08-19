#include "Texture.h"

#include "BindableCodex.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\CommandList.h"
#include "..\DX\ResourceStateTracker.h"

#include <DirectXTex\DirectXTex.h>
#pragma comment(lib,"DirectXTex.lib")
#include <filesystem>


namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;
	namespace fs = std::filesystem;
	namespace dx = DirectX;

	Texture::Texture(Graphics& gfx, const std::wstring& inputPath, std::shared_ptr<DescriptorAllocation> inputDescriptorAllocation)
		:
		Texture(gfx, inputPath, inputDescriptorAllocation, TextureDescription{})
	{
	}
	Texture::Texture(Graphics& gfx, const std::wstring& inputPath, std::shared_ptr<DescriptorAllocation> inputDescriptorAllocation, TextureDescription inputTextureDesc)
		:
		path(WProjectDirectory + inputPath),
		descriptorAllocation(inputDescriptorAllocation),
		textureDesc(inputTextureDesc)
	{
		fs::path filePath(path);

		auto a = fs::absolute(path);

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
				path.c_str(),
				dx::DDS_FLAGS_NONE,
				&metadata,
				scratchRawImage));
		}
		else if (filePath.extension() == ".hdr")
		{
			GFX_THROW_INFO(LoadFromHDRFile(
				path.c_str(),
				&metadata,
				scratchRawImage));
		}
		else if (filePath.extension() == ".tga")
		{
			GFX_THROW_INFO(LoadFromTGAFile(
				path.c_str(),
				&metadata,
				scratchRawImage));
		}
		else
		{
			GFX_THROW_INFO(LoadFromWICFile(
				path.c_str(),
				dx::WIC_FLAGS_NONE,
				&metadata,
				scratchRawImage));
		}

		dx::ScratchImage scratchImage;

		if (textureDesc.generateMip)
		{
			GFX_THROW_INFO(dx::GenerateMipMaps(*scratchRawImage.GetImages(), dx::TEX_FILTER_LINEAR, 0, scratchImage));
		}
		else
		{
			scratchImage = std::move(scratchRawImage);
		}

		const auto& chainBase = *scratchImage.GetImages();
		const D3D12_RESOURCE_DESC texDesc{
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Width = (UINT)chainBase.width,
			.Height = (UINT)chainBase.height,
			.DepthOrArraySize = 1,
			.MipLevels = (UINT16)scratchImage.GetImageCount(),
			.Format = chainBase.format,
			.SampleDesc = {.Count = 1 },
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = D3D12_RESOURCE_FLAG_NONE,
		};

		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
		GFX_THROW_INFO(gfx.GetDecive()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&textureBuffer)
		));

		ResourceStateTracker::AddGlobalResourceState(textureBuffer, D3D12_RESOURCE_STATE_COMMON);

		std::vector<D3D12_SUBRESOURCE_DATA> subresourceData;

		for (int i = 0; i < scratchImage.GetImageCount(); i++)
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
			GFX_THROW_INFO(gfx.GetDecive()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&textureUploadBuffer)
			));

			std::shared_ptr<CommandList> copyCommandList = gfx.GetCommandList(D3D12_COMMAND_LIST_TYPE_COPY);

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

		

		gfx.GetCommandList()->AddTransitionBarrier(textureBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, true);

		const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{
				.Format = textureBuffer->GetDesc().Format,
				.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Texture2D{.MipLevels = textureBuffer->GetDesc().MipLevels },
		};
		gfx.GetDecive()->CreateShaderResourceView(textureBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());
	}

	Texture::~Texture()
	{
		ResourceStateTracker::RemoveGlobalResourceState(textureBuffer);
	}

	void Texture::Bind(Graphics& gfx) noxnd
	{
	}

	std::shared_ptr<Texture> Texture::Resolve(DEGraphics::Graphics& gfx, const std::wstring& path,
		std::shared_ptr<DescriptorAllocation> descriptorAllocation)
	{
		return Codex::Resolve<Texture>(gfx, path, descriptorAllocation);
	}

	std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, const std::wstring& path,
		std::shared_ptr<DescriptorAllocation> descriptorAllocation, TextureDescription textureDesc)
	{
		return Codex::Resolve<Texture>(gfx, path, descriptorAllocation, textureDesc);
	}

	std::string Texture::GenerateUID_(const std::wstring& path)
	{
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + Utility::ToNarrow(path);
	}

	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID(path, descriptorAllocation);
	}
}
