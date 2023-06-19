#pragma once
#include "Bindable.h"
#include "..\Graphics.h"

namespace DiveBomber::BindObj
{
	class RootSignature final : public Bindable
	{
	public:
		RootSignature(DEGraphics::Graphics& inputGfx);
		wrl::ComPtr<ID3D12RootSignature> GetRootSignature() noexcept;
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		static std::shared_ptr<RootSignature> Resolve(DEGraphics::Graphics& gfx);
		static std::string GenerateUID();
		std::string GetUID() const noexcept override;
	private:
		DEGraphics::Graphics& gfx;
		wrl::ComPtr<ID3D12RootSignature> rootSignature;
	};
}