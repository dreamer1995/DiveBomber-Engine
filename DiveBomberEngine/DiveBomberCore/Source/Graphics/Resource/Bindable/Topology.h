#pragma once
#include "..\Resource.h"
#include "Bindable.h"
#include "..\..\GraphicsHeader.h"

namespace DiveBomber::DEResource
{
	class Topology final: public Resource, public Bindable
	{
	public:
		Topology(const std::wstring& inputName, D3D_PRIMITIVE_TOPOLOGY inputType, D3D12_PRIMITIVE_TOPOLOGY_TYPE inputShaderType);
		void Bind() noxnd override;
		[[nodiscard]] D3D_PRIMITIVE_TOPOLOGY GetTopology() noexcept;
		[[nodiscard]] D3D12_PRIMITIVE_TOPOLOGY_TYPE GetShaderTopology() noexcept;
		[[nodiscard]] static std::string GenerateUID(const std::wstring& name, const D3D_PRIMITIVE_TOPOLOGY type, const D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType);
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		D3D_PRIMITIVE_TOPOLOGY type;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType;
	};
}