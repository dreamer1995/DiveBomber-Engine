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

namespace DiveBomber::BindableObject
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
		Texture(DEGraphics::Graphics& gfx, const std::wstring& inputName);
		Texture(DEGraphics::Graphics& gfx, const std::wstring& inputName, TextureDescription inputTextureDesc);
		~Texture();

		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept;
		
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		[[nodiscard]] static std::shared_ptr<Texture> Resolve(DEGraphics::Graphics& gfx, const std::wstring& name);
		[[nodiscard]] static std::shared_ptr<Texture> Resolve(DEGraphics::Graphics& gfx, const std::wstring& name,
			TextureDescription textureDesc);
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::wstring& name, Ignore&&...ignore)
		{
			return GenerateUID_(name);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		[[nodiscard]] static std::string GenerateUID_(const std::wstring& name);
		std::wstring name;
		std::shared_ptr<DX::DescriptorAllocation> descriptorAllocation;
		wrl::ComPtr<ID3D12Resource> textureBuffer;
		TextureDescription textureDesc;
	};
}