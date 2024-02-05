#include "Texture.h"

#include "..\..\..\Utility\GlobalParameters.h"
#include "..\..\GraphicsSource.h"
#include "..\..\DX\GlobalResourceManager.h"
#include "..\..\DX\DescriptorAllocator.h"
#include "..\..\DX\DescriptorAllocation.h"
#include "..\..\DX\CommandList.h"
#include "..\..\DX\ResourceStateTracker.h"
#include "..\ResourceCommonInclude.h"
#include "..\..\..\Component\Material.h"
#include "..\..\DX\CommandQueue.h"

#include <fstream>
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
	using namespace DEComponent;
	namespace fs = std::filesystem;
	namespace dx = DirectX;

	Texture::Texture(const std::wstring& inputName, TextureParam inputTextureDesc)
		:
		Resource(inputName),
		textureParam(inputTextureDesc),
		descriptorAllocation(Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate(1u))
	{
		if (name.contains(L"#DERBIcon"))
		{
			iconLoadMode = true;
		}
		if (iconLoadMode)
		{
			textureParam.generateMip = false;
			textureParam.sRGB = true;
		}
		else
		{
			GetConfig();
		}
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

	D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetSRVDescriptorCPUHandle() const noexcept
	{
		return descriptorAllocation->GetCPUDescriptorHandle();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetSRVDescriptorGPUHandle() const noexcept
	{
		return descriptorAllocation->GetGPUDescriptorHandle();
	}

	wrl::ComPtr<ID3D12Resource> Texture::GetTextureBuffer() const noexcept
	{
		return textureBuffer;
	}

	std::wstring Texture::GetName() const noexcept
	{
		return name;
	}

	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID(ProjectDirectoryW L"Asset\\Texture\\" + name);
	}

	void Texture::GetConfig()
	{
		configFilePath = ProjectDirectoryW L"Asset\\Texture\\" + name;
		configFilePath.replace_extension(".json");
		if (!fs::exists(configFilePath))
		{
			UpdateConfig(textureParam);
		}
		else
		{
			std::ifstream rawFile(configFilePath);
			if (!rawFile.is_open())
			{
				throw std::exception("Unable to open script file");
			}
			rawFile >> config;
			textureParam.sRGB = config["sRGB"];
			textureParam.generateMip = config["GenerateMip"];
			textureParam.cubeMap = config["CubeMap"];
			textureParam.globalIllumination = config["GlobalIllumination"];
			textureParam.textureArray = config["TextureArray"];
			textureParam.texture3D = config["Texture3D"];
			rawFile.close();
		}
	}

	void Texture::UpdateConfig(const TextureParam inputTextureParam)
	{
		textureParam = inputTextureParam;

		config["sRGB"] = inputTextureParam.sRGB;
		config["GenerateMip"] = inputTextureParam.generateMip;
		config["CubeMap"] = inputTextureParam.cubeMap;
		config["GlobalIllumination"] = inputTextureParam.globalIllumination;
		config["TextureArray"] = inputTextureParam.textureArray;
		config["Texture3D"] = inputTextureParam.texture3D;

		// write prettified JSON to another file
		std::ofstream outFile(configFilePath);
		outFile << std::setw(4) << config << std::endl;
		outFile.close();
	}

	void Texture::GenerateCache(const wrl::ComPtr<ID3D12Resource>& outputTextureBuffer, const std::filesystem::path& outputPath, bool cubemap)
	{
		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(outputTextureBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, true);
		Graphics::GetInstance().ExecuteAllCurrentCommandLists();

		DirectX::ScratchImage scratchImage;
		dx::CaptureTexture(Graphics::GetInstance().GetCommandQueue()->GetCommandQueue().Get(), outputTextureBuffer.Get(), cubemap, scratchImage,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

		GenerateCache(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), outputPath);
	}

	void Texture::GenerateCache(const DirectX::Image* images, size_t numImages, const dx::TexMetadata texMetaData, const std::filesystem::path& outputPath)
	{
		HRESULT hr;
		fs::path cachePath(ProjectDirectoryW L"Cache\\Texture\\");
		if (!fs::exists(cachePath))
		{
			fs::create_directories(cachePath);
		}

		GFX_THROW_INFO(dx::SaveToDDSFile(images, numImages, texMetaData, dx::DDS_FLAGS_NONE, outputPath.c_str()));
	}

	void Texture::LoadTexture()
	{
		bool generateCache = false;
		bool generateIconCache = false;
		bool generateMip = false;
		bool generateCube = false;
		bool generateDiffuseIrradiance = false;
		bool generateSpecularMip = false;

		// check what should be load
		fs::path filePath(ProjectDirectoryW L"Asset\\Texture\\" + name);
		fs::path cachePath(ProjectDirectoryW L"Cache\\Texture\\" + name);
		cachePath.replace_extension(".dds");
		if (!fs::exists(cachePath) && !iconLoadMode)
		{
			generateCache = true;
		}
		else
		{
			filePath = cachePath;
		}

		fs::path iconPath(cachePath);
		iconPath.replace_filename(fs::path(name).stem().wstring() + L"#DERBIcon" + cachePath.extension().wstring());
		if (!fs::exists(iconPath) && !iconLoadMode)
		{
			generateIconCache = true;
		}

		fs::path diffuseIrradiancePath(ProjectDirectoryW L"Cache\\Texture\\" + cachePath.stem().wstring() + L"#DiffuseIrradiance.dds");
		if (textureParam.globalIllumination && !fs::exists(diffuseIrradiancePath) && !iconLoadMode)
		{
			generateDiffuseIrradiance = true;
		}

		fs::path specularMipPath(ProjectDirectoryW L"Cache\\Texture\\" + cachePath.stem().wstring() + L"#SpecularIBL.dds");
		if (textureParam.globalIllumination && !fs::exists(specularMipPath) && !iconLoadMode)
		{
			generateSpecularMip = true;
		}

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

		D3D12_RESOURCE_DESC texDesc{
			.Dimension = textureParam.texture3D ? D3D12_RESOURCE_DIMENSION_TEXTURE3D : D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Width = (UINT)metadata.width,
			.Height = (UINT)metadata.height,
			.DepthOrArraySize = (UINT16)metadata.arraySize,
			.MipLevels = textureParam.generateMip ? 0u : 1u,
			.Format = CheckSRGBFormat(metadata.format) ? DXGI_FORMAT_R8G8B8A8_UNORM : metadata.format,
			.SampleDesc = {.Count = 1 },
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = D3D12_RESOURCE_FLAG_NONE,
		};

		bool readSRGB = false;
		if(texDesc.Format == DXGI_FORMAT_R16G16B16A16_FLOAT || texDesc.Format == DXGI_FORMAT_R32G32B32_FLOAT || texDesc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT)
		{
			readSRGB = true;
		}

		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
		GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&textureBuffer)
		));

		std::vector<D3D12_SUBRESOURCE_DATA> subresourceData;

		for (int j = 0; j < metadata.arraySize; j++)
			for (int i = 0; i < metadata.mipLevels; i++)
		{
			const auto img = scratchImage.GetImage(i, j, 0);

			D3D12_SUBRESOURCE_DATA subresourceDataClip = D3D12_SUBRESOURCE_DATA{
				.pData = img->pixels,
				.RowPitch = (LONG_PTR)img->rowPitch,
				.SlicePitch = (LONG_PTR)img->slicePitch,
			};
			subresourceData.emplace_back(subresourceDataClip);
		}

		{
			wrl::ComPtr<ID3D12Resource> textureUploadBuffer;

			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
			const auto uploadBufferSize = GetRequiredIntermediateSize(textureBuffer.Get(), 0u, (UINT)subresourceData.size());
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
				0, 0u,
				(UINT)subresourceData.size(),
				subresourceData.data()
			);

			copyCommandList->TrackResource(textureUploadBuffer);
			
			ResourceStateTracker::AddGlobalResourceState(textureBuffer, D3D12_RESOURCE_STATE_COMMON);
			Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(textureBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, true);
		}

		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		wrl::ComPtr<ID3D12Resource> cubeSourceTextureBuffer;
		// second round, check if should generate mipmap / cubemap
		if (textureParam.cubeMap && metadata.arraySize == 1 && !iconLoadMode)
		{
			generateCube = true;
			generateCache = true;

			cubeSourceTextureBuffer = textureBuffer;

			D3D12_RESOURCE_DESC cubeResDesc = resDesc;
			cubeResDesc.Alignment = 0u;
			cubeResDesc.Width = cubeResDesc.Height / 2;
			cubeResDesc.Height = (UINT)cubeResDesc.Width;
			cubeResDesc.DepthOrArraySize = 6u;
			cubeResDesc.MipLevels = textureParam.generateMip ? 0u : 1u;

			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
			GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&cubeResDesc,
				D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
				nullptr,
				IID_PPV_ARGS(&textureBuffer)
			));

			ResourceStateTracker::AddGlobalResourceState(textureBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
		}

		// second round, check if should generate mipmap / cubemap
		if (textureParam.generateMip && (metadata.mipLevels < resDesc.MipLevels) && !iconLoadMode)
		{
			generateMip = true;
			generateCache = true;
		}

		wrl::ComPtr<ID3D12Resource> aliasBuffer;
		wrl::ComPtr<ID3D12Resource> uavBuffer = textureBuffer;
		// make a saperate uav buffer
		if (generateCube || generateMip)
		{
			D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

			// Describe an alias resource that is used to copy the original texture.
			D3D12_RESOURCE_DESC aliasDesc = resDesc;
			// Placed resources can't be render targets or depth-stencil views.
			aliasDesc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

			// Describe a UAV compatible resource that is used to perform
			// mipmapping of the original texture.
			D3D12_RESOURCE_DESC uavDesc = aliasDesc;   
			// The flags for the UAV description must match that of the alias description.
			uavDesc.Format = GetUAVCompatableFormat(resDesc.Format);
			uavDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

			D3D12_RESOURCE_DESC resourceDescs[] = {
				aliasDesc,
				uavDesc
			};

			// Create a heap that is large enough to store a copy of the original resource.
			D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = Graphics::GetInstance().GetDevice()->GetResourceAllocationInfo(0, _countof(resourceDescs), resourceDescs);

			D3D12_HEAP_DESC heapDesc = {};
			heapDesc.SizeInBytes = allocationInfo.SizeInBytes;
			heapDesc.Alignment = allocationInfo.Alignment;
			heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
			heapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

			wrl::ComPtr<ID3D12Heap> heap;

			HRESULT hr;
			GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap)));

			// Create a placed resource that matches the description of the 
			// original resource. This resource is used to copy the original 
			// texture to the UAV compatible resource.
			GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreatePlacedResource(
				heap.Get(),
				0,
				&aliasDesc,
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(&aliasBuffer)
			));

			ResourceStateTracker::AddGlobalResourceState(aliasBuffer, D3D12_RESOURCE_STATE_COMMON);

			// Create a UAV compatible resource in the same heap as the alias
			// resource.
			GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreatePlacedResource(
				heap.Get(),
				0,
				&uavDesc,
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(&uavBuffer)
			));

			ResourceStateTracker::AddGlobalResourceState(uavBuffer, D3D12_RESOURCE_STATE_COMMON);

			Graphics::GetInstance().GetCommandList()->AliasingBarrier(nullptr, aliasBuffer);

			if (!generateCube)
			{
				// Copy the original resource to the alias resource.
				// This ensures GPU validation.
				Graphics::GetInstance().GetCommandList()->CopyResource(aliasBuffer, textureBuffer);
			}

			Graphics::GetInstance().GetCommandList()->AliasingBarrier(aliasBuffer, uavBuffer);
		}

		if (generateCube)
		{
			GenerateCubeMap(uavBuffer, cubeSourceTextureBuffer, readSRGB);
		}

		if (generateMip)
		{
			GenerateMipMaps(uavBuffer);
		}

		if (generateIconCache)
		{
			if (metadata.arraySize == 1)
			{
				wrl::ComPtr<ID3D12Resource> iconBuffer;
				D3D12_RESOURCE_DESC iconResDesc = resDesc;
				iconResDesc.Alignment = 0u;
				iconResDesc.MipLevels = 0u;
				iconResDesc.Format = GetUAVCompatableFormat(resDesc.Format);
				iconResDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
				GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&iconResDesc,
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(&iconBuffer)
				));

				{
					wrl::ComPtr<ID3D12Resource> textureUploadBuffer;

					const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
					const auto uploadBufferSize = GetRequiredIntermediateSize(iconBuffer.Get(), 0u, (UINT)subresourceData.size());
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
						iconBuffer.Get(),
						textureUploadBuffer.Get(),
						0, 0u,
						(UINT)subresourceData.size(),
						subresourceData.data()
					);

					copyCommandList->TrackResource(textureUploadBuffer);

					ResourceStateTracker::AddGlobalResourceState(iconBuffer, D3D12_RESOURCE_STATE_COMMON);
				}

				GenerateMipMaps(iconBuffer);
				GenerateIconMap(iconBuffer, iconPath);

				ResourceStateTracker::RemoveGlobalResourceState(iconBuffer);
			}
			else
			{
				GenerateIconMap(uavBuffer, iconPath);
			}
		}

		ResourceStateTracker::RemoveGlobalResourceState(cubeSourceTextureBuffer);

		if (generateDiffuseIrradiance)
		{
			GenerateDiffuseIrradiance(uavBuffer, diffuseIrradiancePath);
		}

		if (generateSpecularMip)
		{
			GenerateSpecularIBLMipMaps(uavBuffer, specularMipPath);
		}

		if (aliasBuffer)
		{
			Graphics::GetInstance().GetCommandList()->AliasingBarrier(uavBuffer, aliasBuffer);
			// Copy the alias resource back to the original resource.
			Graphics::GetInstance().GetCommandList()->CopyResource(textureBuffer, aliasBuffer);

			Graphics::GetInstance().ExecuteAllCurrentCommandLists();

			// really needed?
			Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(textureBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, true);

			ResourceStateTracker::RemoveGlobalResourceState(aliasBuffer);
			ResourceStateTracker::RemoveGlobalResourceState(uavBuffer);
		}

		if (generateCache)
		{
			GenerateCache(textureBuffer, cachePath, textureParam.cubeMap);
		}

		resDesc = textureBuffer->GetDesc();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (textureParam.cubeMap)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MipLevels = resDesc.MipLevels;
		}
		else
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = resDesc.MipLevels;
		}

		Graphics::GetInstance().GetDevice()->CreateShaderResourceView(textureBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());
	}

	void Texture::GenerateMipMaps(wrl::ComPtr<ID3D12Resource>& uavBuffer)
	{
		const std::wstring generateMipName(L"GenerateMipLinear");
		D3D12_RESOURCE_DESC resDesc = uavBuffer->GetDesc();

		std::shared_ptr<Material> material = std::make_shared<Material>(generateMipName + L"Material", generateMipName);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MipLevels = resDesc.MipLevels;
		srvDesc.Texture2DArray.ArraySize = resDesc.DepthOrArraySize;

		Graphics::GetInstance().GetDevice()->CreateShaderResourceView(uavBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());

		material->SetTexture(GetSRVDescriptorHeapOffset(), 0);

		std::shared_ptr<RootSignature> rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");

		TextureMipMapGenerateConstant mipGenCB;
		mipGenCB.isSRGB = textureParam.sRGB;
		std::shared_ptr<ConstantBufferInHeap<TextureMipMapGenerateConstant>> mipGenCBIndex =
			std::make_shared<ConstantBufferInHeap<TextureMipMapGenerateConstant>>(generateMipName);
		material->SetConstant(mipGenCBIndex, 1);

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.material = material;

		std::shared_ptr<PipelineStateObject> pipelineStateObject = std::make_shared<PipelineStateObject>(generateMipName, std::move(pipelineStateReference));

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

			Graphics::GetInstance().BindShaderDescriptorHeaps();
			for (uint32_t mip = 0; mip < mipCount; ++mip)
			{
				D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
				uavDesc.Format = resDesc.Format;
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				uavDesc.Texture2DArray.MipSlice = srcMip + mip + 1;
				uavDesc.Texture2DArray.ArraySize = resDesc.DepthOrArraySize;
				std::shared_ptr<UnorderedAccessBuffer> mipTarget = std::make_shared<UnorderedAccessBuffer>(
					Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
					uavBuffer, uavDesc);

				mipTarget->BindAsTarget();
				material->SetTexture(mipTarget, mip + 1);
			}

			rootSignature->Bind();
			material->Bind();
			pipelineStateObject->Bind();

			Graphics::GetInstance().GetGraphicsCommandList()->Dispatch(
				(dstWidth + 7u) / 8u,
				(dstHeight + 7u) / 8u,
				(UINT)resDesc.DepthOrArraySize);

			srcMip += mipCount;
		}

		Graphics::GetInstance().ExecuteAllCurrentCommandLists();
	}

	void Texture::GenerateDiffuseIrradiance(wrl::ComPtr<ID3D12Resource>& uavBuffer, const std::filesystem::path& outputPath)
	{
		HRESULT hr;

		wrl::ComPtr<ID3D12Resource> outputCubeTarget;

		// Create final cube resource
		D3D12_RESOURCE_DESC resDesc = uavBuffer->GetDesc();
		resDesc.Alignment = 0u;
		resDesc.MipLevels = 1u;
		resDesc.Format = GetUAVCompatableFormat(resDesc.Format);
		resDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
		GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&outputCubeTarget)
		));

		ResourceStateTracker::AddGlobalResourceState(outputCubeTarget, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		const std::wstring generateMipName(L"GenerateDiffuseIrradiance");

		std::shared_ptr<Material> material = std::make_shared<Material>(generateMipName + L"Material", generateMipName);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = resDesc.MipLevels;

		Graphics::GetInstance().GetDevice()->CreateShaderResourceView(uavBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());

		material->SetTexture(GetSRVDescriptorHeapOffset(), 0);

		std::shared_ptr<RootSignature> rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");

		resDesc = outputCubeTarget->GetDesc();

		TextureDiffuseMipGenerateConstant mipGenCB;
		mipGenCB.isSRGB = textureParam.sRGB;
		std::shared_ptr<ConstantBufferInHeap<TextureDiffuseMipGenerateConstant>> mipGenCBIndex =
			std::make_shared<ConstantBufferInHeap<TextureDiffuseMipGenerateConstant>>(generateMipName);
		material->SetConstant(mipGenCBIndex, 1);

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.material = material;

		std::shared_ptr<PipelineStateObject> pipelineStateObject = std::make_shared<PipelineStateObject>(generateMipName, std::move(pipelineStateReference));

		Graphics::GetInstance().BindShaderDescriptorHeaps();

		mipGenCB.texelSize.x = 1.0f / (float)resDesc.Width;
		mipGenCB.texelSize.y = 1.0f / (float)resDesc.Height;

		mipGenCBIndex->Update(mipGenCB);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.Format = resDesc.Format;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.MipSlice = 0u;
		uavDesc.Texture2DArray.ArraySize = 6u;
		std::shared_ptr<UnorderedAccessBuffer> mipTarget = std::make_shared<UnorderedAccessBuffer>(
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			outputCubeTarget, uavDesc);

		mipTarget->BindAsTarget();
		material->SetTexture(mipTarget, 1u);

		rootSignature->Bind();
		material->Bind();
		pipelineStateObject->Bind();

		Graphics::GetInstance().GetGraphicsCommandList()->Dispatch(
			((UINT)resDesc.Width + 7u) / 8u,
			((UINT)resDesc.Height + 7u) / 8u,
			6u);

		GenerateCache(outputCubeTarget, outputPath, true);

		ResourceStateTracker::RemoveGlobalResourceState(outputCubeTarget);
	}

	void Texture::GenerateSpecularIBLMipMaps(wrl::ComPtr<ID3D12Resource>& uavBuffer, const std::filesystem::path& outputPath)
	{
		HRESULT hr;
		
		wrl::ComPtr<ID3D12Resource> outputCubeTarget;

		// Create final cube resource
		D3D12_RESOURCE_DESC resDesc = uavBuffer->GetDesc();
		resDesc.Alignment = 0u;
		resDesc.Format = GetUAVCompatableFormat(resDesc.Format);
		resDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
		GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&outputCubeTarget)
		));

		ResourceStateTracker::AddGlobalResourceState(outputCubeTarget, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		const std::wstring generateMipName(L"GenerateSpecularIBLMip");

		std::shared_ptr<Material> material = std::make_shared<Material>(generateMipName + L"Material", generateMipName);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = resDesc.MipLevels;

		Graphics::GetInstance().GetDevice()->CreateShaderResourceView(uavBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());

		material->SetTexture(GetSRVDescriptorHeapOffset(), 0);

		std::shared_ptr<RootSignature> rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");

		resDesc = outputCubeTarget->GetDesc();

		TextureSpecularMipGenerateConstant mipGenCB;
		mipGenCB.isSRGB = textureParam.sRGB;
		std::shared_ptr<ConstantBufferInHeap<TextureSpecularMipGenerateConstant>> mipGenCBIndex =
			std::make_shared<ConstantBufferInHeap<TextureSpecularMipGenerateConstant>>(generateMipName);
		material->SetConstant(mipGenCBIndex, 1);

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.material = material;

		std::shared_ptr<PipelineStateObject> pipelineStateObject = std::make_shared<PipelineStateObject>(generateMipName, std::move(pipelineStateReference));

		Graphics::GetInstance().BindShaderDescriptorHeaps();
		for (int srcMip = 0; srcMip < resDesc.MipLevels; srcMip++)
		{
			UINT dstWidth = (UINT)resDesc.Width >> srcMip;
			UINT dstHeight = resDesc.Height >> srcMip;

			mipGenCB.roughness = srcMip / (float)(resDesc.MipLevels - 1);
			mipGenCB.texelSize.x = 1.0f / (float)dstWidth;
			mipGenCB.texelSize.y = 1.0f / (float)dstHeight;

			mipGenCBIndex->Update(mipGenCB);

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
			uavDesc.Format = resDesc.Format;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.Texture2DArray.MipSlice = srcMip;
			uavDesc.Texture2DArray.ArraySize = 6u;
			std::shared_ptr<UnorderedAccessBuffer> mipTarget = std::make_shared<UnorderedAccessBuffer>(
				Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
				outputCubeTarget, uavDesc);

			mipTarget->BindAsTarget();
			material->SetTexture(mipTarget, 1u);

			rootSignature->Bind();
			material->Bind();
			pipelineStateObject->Bind();

			Graphics::GetInstance().GetGraphicsCommandList()->Dispatch(
				(dstWidth + 7u) / 8u,
				(dstHeight + 7u) / 8u,
				6u);
		}

		GenerateCache(outputCubeTarget, outputPath, true);

		ResourceStateTracker::RemoveGlobalResourceState(outputCubeTarget);
	}

	void Texture::GenerateCubeMap(wrl::ComPtr<ID3D12Resource>& uavBuffer, wrl::ComPtr<ID3D12Resource>& cubeSourceTextureBuffer, bool readSRGB)
	{
		// Create final cube resource
		D3D12_RESOURCE_DESC resDesc = uavBuffer->GetDesc();

		// Create render request resource
		const std::wstring generateCubeName(L"GenerateCubeMap");

		std::shared_ptr<Material> material = std::make_shared<Material>(generateCubeName, generateCubeName);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = cubeSourceTextureBuffer->GetDesc().Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1u;

		Graphics::GetInstance().GetDevice()->CreateShaderResourceView(cubeSourceTextureBuffer.Get(), &srvDesc, descriptorAllocation->GetCPUDescriptorHandle());

		material->SetTexture(GetSRVDescriptorHeapOffset(), 0);

		std::shared_ptr<RootSignature> rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");

		TextureCubeMapGenerateConstant cubeGenCB;
		std::shared_ptr<ConstantBufferInHeap<TextureCubeMapGenerateConstant>> cubeGenCBIndex =
			std::make_shared<ConstantBufferInHeap<TextureCubeMapGenerateConstant>>(generateCubeName);

		material->SetConstant(cubeGenCBIndex, 1);

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.material = material;

		std::shared_ptr<PipelineStateObject> pipelineStateObject = std::make_shared<PipelineStateObject>(generateCubeName, std::move(pipelineStateReference));

		Graphics::GetInstance().BindShaderDescriptorHeaps();

		cubeGenCB.readSRGB = readSRGB;
		cubeGenCB.texelSize.x = 1.0f / (float)resDesc.Width;
		cubeGenCB.texelSize.y = 1.0f / (float)resDesc.Height;
		cubeGenCBIndex->Update(cubeGenCB);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.Format = resDesc.Format;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.ArraySize = 6u;
		uavDesc.Texture2DArray.FirstArraySlice = 0u;
		uavDesc.Texture2DArray.MipSlice = 0;

		std::shared_ptr<UnorderedAccessBuffer> mipTarget = std::make_shared<UnorderedAccessBuffer>(
			Graphics::GetInstance().GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			uavBuffer, uavDesc);

		mipTarget->BindAsTarget();
		material->SetTexture(mipTarget, 1u);

		rootSignature->Bind();
		material->Bind();
		pipelineStateObject->Bind();

		Graphics::GetInstance().GetGraphicsCommandList()->Dispatch(
			((UINT)resDesc.Width + 7u) / 8,
			((UINT)resDesc.Height + 7u) / 8,
			6u);

		Graphics::GetInstance().ExecuteAllCurrentCommandLists();
	}

	void Texture::GenerateIconMap(wrl::ComPtr<ID3D12Resource>& uavBuffer, const std::filesystem::path& outputPath)
	{
		// Create final icon resource
		D3D12_RESOURCE_DESC resDesc = uavBuffer->GetDesc();
		const DirectX::Image* iconImage;
		DirectX::ScratchImage scratchImage;

		Graphics::GetInstance().GetCommandList()->AddTransitionBarrier(uavBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, true);
		Graphics::GetInstance().ExecuteAllCurrentCommandLists();

		dx::CaptureTexture(Graphics::GetInstance().GetCommandQueue()->GetCommandQueue().Get(), uavBuffer.Get(), false, scratchImage,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

		UINT dstMip = 0;
		while (resDesc.Width > 64)
		{
			resDesc.Width >>= 1;
			dstMip++;
		}

		iconImage = scratchImage.GetImage(dstMip, 0, 0);

		DirectX::TexMetadata iconTexMetaData = scratchImage.GetMetadata();
		iconTexMetaData.width = iconImage->width;
		iconTexMetaData.height = iconImage->height;
		iconTexMetaData.arraySize = 1u;
		iconTexMetaData.depth = 1u;
		iconTexMetaData.mipLevels = 1u;
		iconTexMetaData.dimension = dx::TEX_DIMENSION_TEXTURE2D;

		GenerateCache(iconImage, 1u, iconTexMetaData, outputPath);
	}

	DXGI_FORMAT Texture::GetUAVCompatableFormat(DXGI_FORMAT format)
	{
		DXGI_FORMAT uavFormat = format;

		switch (uavFormat)
		{
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC7_TYPELESS:
			uavFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
			break;
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC7_UNORM:
			uavFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case DXGI_FORMAT_BC4_TYPELESS:
			uavFormat = DXGI_FORMAT_R8_TYPELESS;
			break;
		case DXGI_FORMAT_BC4_UNORM:
			uavFormat = DXGI_FORMAT_R8_UNORM;
			break;
		case DXGI_FORMAT_BC4_SNORM:
			uavFormat = DXGI_FORMAT_R8_SNORM;
			break;
		case DXGI_FORMAT_BC5_TYPELESS:
			uavFormat = DXGI_FORMAT_R8G8_TYPELESS;
			break;
		case DXGI_FORMAT_BC5_UNORM:
			uavFormat = DXGI_FORMAT_R8G8_UNORM;
			break;
		case DXGI_FORMAT_BC5_SNORM:
			uavFormat = DXGI_FORMAT_R8G8_SNORM;
			break;
		case DXGI_FORMAT_BC6H_TYPELESS:
			uavFormat = DXGI_FORMAT_R16G16B16A16_TYPELESS;
			break;
		case DXGI_FORMAT_BC6H_UF16:
			uavFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
			break;
		case DXGI_FORMAT_BC6H_SF16:
			uavFormat = DXGI_FORMAT_R16G16B16A16_SNORM;
			break;
		}

		return uavFormat;
	}

	bool Texture::CheckSRGBFormat(DXGI_FORMAT format)
	{
		bool isSRGB = false;

		switch (format)
		{
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			isSRGB = true;
		}

		return isSRGB;
	}
}