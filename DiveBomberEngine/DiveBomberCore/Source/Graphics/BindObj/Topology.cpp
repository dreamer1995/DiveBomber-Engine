#include "Topology.h"

#include "BindableCodex.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	Topology::Topology(Graphics& gfx, D3D12_PRIMITIVE_TOPOLOGY inputType)
	{
		type = inputType;
	}

	void Topology::Bind(Graphics& gfx) noxnd
	{
		GFX_THROW_INFO_ONLY(gfx.GetCommandList()->IASetPrimitiveTopology(type));
	}

	D3D12_PRIMITIVE_TOPOLOGY Topology::GetTopology() noexcept
	{
		return type;
	}

	std::shared_ptr<Topology> Topology::Resolve(Graphics& gfx, D3D12_PRIMITIVE_TOPOLOGY type)
	{
		return Codex::Resolve<Topology>(gfx, type);
	}

	std::string Topology::GenerateUID(D3D12_PRIMITIVE_TOPOLOGY type)
	{
		using namespace std::string_literals;
		return typeid(Topology).name() + "#"s + std::to_string(type);
	}

	std::string Topology::GetUID() const noexcept
	{
		return GenerateUID(type);
	}
}
