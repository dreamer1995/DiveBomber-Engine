#include "Topology.h"

#include "..\..\DiveBomberCore.h"
#include "..\Graphics.h"
#include "GlobalBindableManager.h"
#include "..\..\Exception\GraphicsException.h"

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;

	Topology::Topology(D3D_PRIMITIVE_TOPOLOGY inputType, D3D12_PRIMITIVE_TOPOLOGY_TYPE inputShaderType)
		:
		type(inputType),
		shaderType(inputShaderType)
	{
	}

	void Topology::Bind() noxnd
	{
		GFX_THROW_INFO_ONLY(Graphics::GetInstance().GetGraphicsCommandList()->IASetPrimitiveTopology(type));
	}

	D3D12_PRIMITIVE_TOPOLOGY Topology::GetTopology() noexcept
	{
		return type;
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE Topology::GetShaderTopology() noexcept
	{
		return shaderType;
	}

	std::shared_ptr<Topology> Topology::Resolve(const D3D_PRIMITIVE_TOPOLOGY type, const D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType)
	{
		return GlobalBindableManager::Resolve<Topology>(type, shaderType);
	}

	std::string Topology::GenerateUID(const D3D_PRIMITIVE_TOPOLOGY type, const D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType)
	{
		using namespace std::string_literals;
		return typeid(Topology).name() + "#"s + "Type"s + std::to_string(type) + "#"s + "ShaderType"s + std::to_string(type);
	}

	std::string Topology::GetUID() const noexcept
	{
		return GenerateUID(type, shaderType);
	}
}
