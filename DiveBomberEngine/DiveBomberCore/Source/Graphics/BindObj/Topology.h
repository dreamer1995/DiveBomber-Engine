#pragma once
#include "Bindable.h"
#include "..\Graphics.h"

namespace DiveBomber::BindObj
{
	class Topology final: public Bindable
	{
	public:
		Topology(DEGraphics::Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY inputType, D3D12_PRIMITIVE_TOPOLOGY_TYPE inputShaderType);
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		D3D_PRIMITIVE_TOPOLOGY GetTopology() noexcept;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE GetShaderTopology() noexcept;
		static std::shared_ptr<Topology> Resolve(DEGraphics::Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		static std::string GenerateUID(D3D_PRIMITIVE_TOPOLOGY type, D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType);
		std::string GetUID() const noexcept override;
	private:
		D3D_PRIMITIVE_TOPOLOGY type;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType;
	};
}