#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

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
		Texture(const std::wstring& inputName);
		Texture(const std::wstring& inputName, TextureDescription inputTextureDesc);
		~Texture();

		[[nodiscard]] UINT GetSRVDescriptorHeapOffset() const noexcept;
		
		void Bind() noxnd override;
		[[nodiscard]] static std::shared_ptr<Texture> Resolve(const std::wstring& name);
		[[nodiscard]] static std::shared_ptr<Texture> Resolve(const std::wstring& name,
			TextureDescription textureDesc);
		std::wstring GetName() const noexcept;
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