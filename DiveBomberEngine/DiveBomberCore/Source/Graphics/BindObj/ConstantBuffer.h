#pragma once
#include "Bindable.h"
#include "..\Graphics.h"

namespace DiveBomber::BindObj
{
	class ConstantBuffer final: public Bindable
	{
	public:
		ConstantBuffer(DEGraphics::Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY inputType, D3D12_PRIMITIVE_TOPOLOGY_TYPE inputShaderType);
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		[[nodiscard]] D3D12_PRIMITIVE_TOPOLOGY_TYPE GetShaderTopology() noexcept;
		[[nodiscard]] static std::shared_ptr<ConstantBuffer> Resolve(DEGraphics::Graphics& gfx, const D3D_PRIMITIVE_TOPOLOGY type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			const D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		[[nodiscard]] static std::string GenerateUID(const D3D_PRIMITIVE_TOPOLOGY type, const D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType);
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		D3D_PRIMITIVE_TOPOLOGY type;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType;
	};
}