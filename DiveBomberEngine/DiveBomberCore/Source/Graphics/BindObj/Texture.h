#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::Component
{
	class DescriptorAllocation;
}

namespace DiveBomber::BindObj
{
	class Texture final: public Bindable
	{
	public:
		Texture(DEGraphics::Graphics& gfx, const std::wstring& inputPath, std::shared_ptr<Component::DescriptorAllocation> inputDescriptorAllocation);
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		[[nodiscard]] static std::shared_ptr<Texture> Resolve(DEGraphics::Graphics& gfx, const std::wstring& path,
			std::shared_ptr<Component::DescriptorAllocation> descriptorAllocation);
		[[nodiscard]] static std::string GenerateUID(const std::wstring& path, std::shared_ptr<Component::DescriptorAllocation> descriptorAllocation);
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		std::wstring path;
		std::shared_ptr<Component::DescriptorAllocation> descriptorAllocation;
		wrl::ComPtr<ID3D12Resource> textureBuffer;
		wrl::ComPtr<ID3D12Resource> textureUploadBuffer;
	};
}