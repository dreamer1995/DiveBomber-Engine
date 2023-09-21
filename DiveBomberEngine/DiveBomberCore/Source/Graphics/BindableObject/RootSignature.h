#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::BindableObject
{
	class RootSignature final : public Bindable
	{
	public:
		RootSignature(const std::string& inputTag);
		[[nodiscard]] wrl::ComPtr<ID3D12RootSignature> GetRootSignature() noexcept;
		void Bind() noxnd override;
		[[nodiscard]] static std::shared_ptr<RootSignature> Resolve(const std::string& tag);
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		[[nodiscard]] static std::string GenerateUID_(const std::string& tag);
		std::string tag;
		wrl::ComPtr<ID3D12RootSignature> rootSignature;
	};
}