#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::DX
{
	class DescriptorAllocation;
}

namespace DiveBomber::BindObj
{
	class Texture final: public Bindable
	{
	public:
		struct TextureDescription
		{
			bool sRGB = false;
			bool generateMip = true;
		};
	public:
		Texture(DEGraphics::Graphics& gfx, const std::wstring& inputPath, std::shared_ptr<DX::DescriptorAllocation> inputDescriptorAllocation);
		Texture(DEGraphics::Graphics& gfx, const std::wstring& inputPath, std::shared_ptr<DX::DescriptorAllocation> inputDescriptorAllocation, TextureDescription inputTextureDesc);
		~Texture();
		
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		[[nodiscard]] static std::shared_ptr<Texture> Resolve(DEGraphics::Graphics& gfx, const std::wstring& path,
			std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation);
		[[nodiscard]] static std::shared_ptr<Texture> Resolve(DEGraphics::Graphics& gfx, const std::wstring& path,
			std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation, TextureDescription textureDesc);
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::wstring& path, Ignore&&...ignore)
		{
			return GenerateUID_(path);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		[[nodiscard]] static std::string GenerateUID_(const std::wstring& path);
		std::wstring path;
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		wrl::ComPtr<ID3D12Resource> textureBuffer;
		TextureDescription textureDesc;
	};
}