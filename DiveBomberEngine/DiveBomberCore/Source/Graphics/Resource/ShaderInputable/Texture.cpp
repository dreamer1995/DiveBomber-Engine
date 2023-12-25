#include "Texture.h"

#include "..\..\..\Utility\GlobalParameters.h"
#include "..\..\GraphicsSource.h"
#include "..\..\DX\GlobalResourceManager.h"
#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"
#include "..\..\DX\CommandList.h"
#include "..\..\DX\ResourceStateTracker.h"
#include "..\ResourceCommonInclude.h"
#include "..\..\Component\Material.h"
#include "..\..\DX\CommandQueue.h"

#include <..\DirectXTex\Auxiliary\DirectXTexEXR.h>
#pragma comment(lib,"DirectXTex.lib")

#pragma comment(lib,"Iex-3_2.lib")
#pragma comment(lib,"IlmThread-3_2.lib")
#pragma comment(lib,"Imath-3_2.lib")
#pragma comment(lib,"OpenEXR-3_2.lib")
#pragma comment(lib,"OpenEXRCore-3_2.lib")
#pragma comment(lib,"OpenEXRUtil-3_2.lib")

namespace DiveBomber::DEResource
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;
	using namespace Component;
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
		textureDesc(inputTextureDesc),
		descriptorAllocation(Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate(1u))
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
		return GenerateUID(ProjectDirectoryW L"Asset\\Texture\\" + name);
	}

	void Texture::LoadTexture()
	{
		fs::path cachePath(ProjectDirectoryW L"Cache\\Texture\\" + name);
		cachePath.replace_extension(".dds");
		if (/*fs::exists(cachePath)*/0)
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
		dx::ScratchImage scratchImage;

		HRESULT hr;

		if (filePath.extension() == ".dds")
		{
			GFX_THROW_INFO(LoadFromDDSFile(
				filePath.c_str(),
				dx::DDS_FLAGS_NONE,
				&metadata,
				scratchImage));
		}
		else if (filePath.extension() == ".hdr")
		{
			GFX_THROW_INFO(LoadFromHDRFile(
				filePath.c_str(),
				&metadata,
				scratchImage));
		}
		else if (filePath.extension() == ".tga")
		{
			GFX_THROW_INFO(LoadFromTGAFile(
				filePath.c_str(),
				&metadata,
				scratchImage));
		}
		else if (filePath.extension() == ".exr")
		{
			GFX_THROW_INFO(LoadFromEXRFile(
				filePath.c_str(),
				&metadata,
				scratchImage));
		}
		else
		{
			GFX_THROW_INFO(LoadFromWICFile(
				filePath.c_str(),
				dx::WIC_FLAGS_NONE,
				&metadata,
				scratchImage));
		}

		LoadScratchImage(scratchImage);
		//GenerateCache(scratchImage);
	}

	void Texture::GenerateCache(const DirectX::ScratchImage& scratchImage)
	{
		dx::TexMetadata metadata = scratchImage.GetMetadata();
		metadata.mipLevels = scratchImage.GetImageCount();

		fs::path cachePath(ProjectDirectoryW L"Cache\\Texture\\");
		fs::path fileName(name);
		if (!fs::exists(cachePath))
		{
			fs::create_directories(cachePath);
		}

		fileName.replace_extension(".dds");
		dx::SaveToDDSFile(scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, dx::DDS_FLAGS_NONE, (cachePath.wstring() + fileName.wstring()).c_str());
	}

	void Texture::LoadScratchImage(const dx::ScratchImage& scratchImage)
	{
		const auto& chainBase = *scratchImage.GetImages();

		DXGI_FORMAT format = chainBase.format;
		bool generrateMipNotSupport = dx::IsCompressed(format) || dx::IsTypeless(format) ||
			dx::IsPlanar(format) || dx::IsPalettized(format);

		D3D12_RESOURCE_DESC texDesc{
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Width = (UINT)chainBase.width,
			.Height = (UINT)chainBase.height,
			.DepthOrArraySize = 1u,
			.MipLevels = 0u,
			.Format = chainBase.format,
			.SampleDesc = {.Count = 1 },
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = generrateMipNotSupport ? D3D12_RESOURCE_FLAG_NONE : D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
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
			Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(textureBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, true);
		}

		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		texDesc.MipLevels = (UINT)subresourceData.size();

		if (textureDesc.generateMip && !generrateMipNotSupport && subresourceData.size() < resDesc.MipLevels)
		{
			const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{
				.Format = texDesc.Format,
				.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Texture2D{.MipLevels = texDesc.MipLevels },
			};

			Graphics::GetInstance().GetDevice()->CreateShaderResourceView(textureBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());
			
			GenerateMipMaps();

			texDesc.MipLevels = resDesc.MipLevels;
		}

		const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{
				.Format = texDesc.Format,
				.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Texture2D{.MipLevels = texDesc.MipLevels },
		};

		Graphics::GetInstance().GetDevice()->CreateShaderResourceView(textureBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());
	}

	void Texture::GenerateMipMaps()
	{
		const std::wstring generateMipName(L"GenerateMipLinear");
		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		std::shared_ptr<Material> material = std::make_shared<Material>(generateMipName, L"GenerateMipLinear");
		material->SetTexture(GetSRVDescriptorHeapOffset(), 0);

		std::shared_ptr<RootSignature> rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");
		
		TextureMipMapGenerateConstant mipGenCB;
		mipGenCB.isSRGB = textureDesc.sRGB;
		std::shared_ptr<ConstantBufferInHeap<TextureMipMapGenerateConstant>> mipGenCBIndex =
			std::make_shared<ConstantBufferInHeap<TextureMipMapGenerateConstant>>(generateMipName);
		material->SetConstant(mipGenCBIndex, 1);

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.material = material;

		std::shared_ptr<PipelineStateObject> pipelineStateObject = std::make_shared<PipelineStateObject>(generateMipName, std::move(pipelineStateReference));

		Graphics::GetInstance().BindShaderDescriptorHeaps();
		for (int srcMip = 0; srcMip < resDesc.MipLevels - 1;)
		{
			UINT srcWidth = (UINT)resDesc.Width >> srcMip;
			UINT srcHeight = resDesc.Height >> srcMip;
			UINT dstWidth = srcWidth >> 1;
			UINT dstHeight = srcHeight >> 1;

			// 0b00(0): Both width and height are even.
			// 0b01(1): Width is odd, height is even.
			// 0b10(2): Width is even, height is odd.
			// 0b11(3): Both width and height are odd.
			mipGenCB.srcDimension = (srcHeight & 1) << 1 | (srcWidth & 1);

			// How many mipmap levels to compute this pass (max 4 mips per pass)
			DWORD mipCount;

			// The number of times we can half the size of the texture and get
			// exactly a 50% reduction in size.
			// A 1 bit in the width or height indicates an odd dimension.
			// The case where either the width or the height is exactly 1 is handled
			// as a special case (as the dimension does not require reduction).
			_BitScanForward(&mipCount, (dstWidth == 1 ? dstHeight : dstWidth) |
				(dstHeight == 1 ? dstWidth : dstHeight));
			// Maximum number of mips to generate is 4.
			mipCount = std::min<DWORD>(4, mipCount + 1);
			
			// Clamp to total number of mips left over.
			mipCount = (srcMip + mipCount) >= resDesc.MipLevels ?
				resDesc.MipLevels - srcMip - 1 : mipCount;

			// Dimensions should not reduce to 0.
			// This can happen if the width and height are not the same.
			dstWidth = std::max<DWORD>(1, dstWidth);
			dstHeight = std::max<DWORD>(1, dstHeight);

			mipGenCB.srcMipLevel = srcMip;
			mipGenCB.numMipLevels = mipCount;
			mipGenCB.texelSize.x = 1.0f / (float)dstWidth;
			mipGenCB.texelSize.y = 1.0f / (float)dstHeight;

			mipGenCBIndex->Update(mipGenCB);

			for (uint32_t mip = 0; mip < mipCount; ++mip)
			{
				D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
				uavDesc.Format = resDesc.Format;
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				uavDesc.Texture2D.MipSlice = srcMip + mip + 1;
				std::shared_ptr<UnorderedAccessBuffer> mipTarget = std::make_shared<UnorderedAccessBuffer>(
					Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
					textureBuffer, uavDesc);

				mipTarget->BindAsTarget();
				material->SetTexture(mipTarget, mip + 1);
			}
			
			rootSignature->Bind();
			material->Bind();
			pipelineStateObject->Bind();

			Graphics::GetInstance().GetGraphicsCommandList()->Dispatch(
				(dstWidth + 7u) / 8,
				(dstHeight + 7u) / 8,
				1u);

			srcMip += mipCount;
		}

		Graphics::GetInstance().ExecuteAllCurrentCommandLists();
	}
}
