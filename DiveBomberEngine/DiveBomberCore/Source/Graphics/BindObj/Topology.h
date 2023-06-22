#pragma once
#include "Bindable.h"
#include "..\Graphics.h"

namespace DiveBomber::BindObj
{
	class Topology final: public Bindable
	{
	public:
		Topology(DEGraphics::Graphics& gfx, D3D12_PRIMITIVE_TOPOLOGY inputType);
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		D3D12_PRIMITIVE_TOPOLOGY GetTopology() noexcept;
		static std::shared_ptr<Topology> Resolve(DEGraphics::Graphics& gfx, D3D12_PRIMITIVE_TOPOLOGY type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		static std::string GenerateUID(D3D12_PRIMITIVE_TOPOLOGY type);
		std::string GetUID() const noexcept override;
	private:
		D3D12_PRIMITIVE_TOPOLOGY type;
	};
}