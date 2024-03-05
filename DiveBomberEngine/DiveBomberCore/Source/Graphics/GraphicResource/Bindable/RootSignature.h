#pragma once
#include "..\..\..\Resource.h"
#include "Bindable.h"
#include "..\..\GraphicsHeader.h"

namespace DiveBomber::GraphicResource
{
	class RootSignature final : public DiveBomber::Resource, public Bindable
	{
	public:
		RootSignature(const std::wstring& inputName);
		[[nodiscard]] wrl::ComPtr<ID3D12RootSignature> GetRootSignature() noexcept;
		void Bind() noxnd override;
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::wstring& name, Ignore&&...ignore)
		{
			using namespace std::string_literals;
			return typeid(RootSignature).name() + "#"s + Utility::ToNarrow(name);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		wrl::ComPtr<ID3D12RootSignature> rootSignature;
	};
}