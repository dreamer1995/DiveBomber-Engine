#include "Topology.h"

#include "..\..\GraphicsSource.h"

namespace DiveBomber::DEResource
{
	using namespace DEGraphics;
	using namespace DEException;

	Topology::Topology(const std::wstring& inputName, D3D_PRIMITIVE_TOPOLOGY inputType, D3D12_PRIMITIVE_TOPOLOGY_TYPE inputShaderType)
		:
		Resource(inputName),
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

	std::string Topology::GenerateUID(const std::wstring& name, const D3D_PRIMITIVE_TOPOLOGY type, const D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType)
	{
		using namespace std::string_literals;
		return typeid(Topology).name() + "#"s + Utility::ToNarrow(name) + "#"s + "Type"s + std::to_string(type) + "#"s + "ShaderType"s + std::to_string(type);
	}

	std::string Topology::GetUID() const noexcept
	{
		return GenerateUID(name, type, shaderType);
	}
}
