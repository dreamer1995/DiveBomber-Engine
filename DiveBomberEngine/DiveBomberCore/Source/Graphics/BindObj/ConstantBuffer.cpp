#include "ConstantBuffer.h"

#include "BindableCodex.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	ConstantBuffer::ConstantBuffer(Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY inputType, D3D12_PRIMITIVE_TOPOLOGY_TYPE inputShaderType)
	{
		type = inputType;
		shaderType = inputShaderType;
	}

	void ConstantBuffer::Bind(Graphics& gfx) noxnd
	{
		GFX_THROW_INFO_ONLY(gfx.GetCommandList()->IASetPrimitiveTopology(type));
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE ConstantBuffer::GetShaderTopology() noexcept
	{
		return shaderType;
	}

	std::shared_ptr<ConstantBuffer> ConstantBuffer::Resolve(Graphics& gfx, const D3D_PRIMITIVE_TOPOLOGY type, const D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType)
	{
		return Codex::Resolve<ConstantBuffer>(gfx, type, shaderType);
	}

	std::string ConstantBuffer::GenerateUID(const D3D_PRIMITIVE_TOPOLOGY type, const D3D12_PRIMITIVE_TOPOLOGY_TYPE shaderType)
	{
		using namespace std::string_literals;
		return typeid(ConstantBuffer).name() + "#"s + "Type"s + std::to_string(type) + "#"s + "ShaderType"s + std::to_string(type);
	}

	std::string ConstantBuffer::GetUID() const noexcept
	{
		return GenerateUID(type, shaderType);
	}
}
