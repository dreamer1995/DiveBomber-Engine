#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::BindableObject
{
	class Topology final: public Bindable
	{
	public:
		Topology(D3D_PRIMITIVE_TOPOLOGY inputType, D3D12_PRIMITIVE_TOPOLOGY_TYPE inputShaderType);
		void Bind() noxnd override;
		[[nodiscard]] D3D_PRIMITIVE_TOPOLOGY GetTopology() noexcept;
		[[nodiscard]] D3D12_PRIMITIVE_TOPOLOGY_TYPE GetShaderTopology() noexcept;
		[[nodiscard]] static std::shared_ptr<Topology> Resolve(const D3D_PRIMITIVE_TOPOLOGY type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			const D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		[[nodiscard]] static std::string GenerateUID(const D3D_PRIMITIVE_TOPOLOGY type, const D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType);
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		D3D_PRIMITIVE_TOPOLOGY type;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType;
	};
}