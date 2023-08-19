#include "SimpleSphere.h"

#include "..\Graphics.h"
#include "..\BindableObject\BindableObjectCommon.h"
#include "..\BindableObject\Geometry\Sphere.h"
#include "..\Component\Camera.h"
#include "..\DX\CommandQueue.h"
#include "..\BindableObject\Texture.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::DrawableObject
{
	using namespace DEGraphics;
	using namespace DiveBomber::BindableObject;
	using namespace DiveBomber::BindableObject::VertexProcess;
	using namespace DX;
	namespace dx = DirectX;

	SimpleSphere::SimpleSphere(Graphics& gfx, const std::wstring inputName)
	{
		name = inputName;

		VertexLayout vl;
		vl.Append(VertexLayout::Position3D);
		vl.Append(VertexLayout::Normal);
		vl.Append(VertexLayout::Tangent);
		vl.Append(VertexLayout::Binormal);
		vl.Append(VertexLayout::Texture2D);

		IndexedTriangleList mesh = Sphere::MakeNormalUVed(vl, true);
		mesh.Transform(dx::XMMatrixScaling(1, 1, 1));

		const std::string geometryTag = Utility::ToNarrow(name);

		std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(gfx, geometryTag, mesh.vertices);
		AddBindable(vertexBuffer);
		indexBuffer = std::make_shared<IndexBuffer>(gfx, geometryTag, mesh.indices);
		AddBindable(indexBuffer);

		std::shared_ptr<DescriptorAllocation> descriptorAllocation =
			gfx.GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate(1u);
		AddBindable(std::make_shared<Texture>(gfx, L"earth.dds", std::move(descriptorAllocation)));

		descriptorAllocation =
			gfx.GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate(1u);
		AddBindable(std::make_shared<Texture>(gfx, L"rustediron2_basecolor.png", std::move(descriptorAllocation)));

		std::shared_ptr<Shader> vertexShader = Shader::Resolve(gfx, L"VShader", Shader::ShaderType::VertexShader);
		AddBindable(vertexShader);
		std::shared_ptr<Shader> pixelShader = Shader::Resolve(gfx, L"PShader", Shader::ShaderType::PixelShader);
		AddBindable(pixelShader);

		std::shared_ptr<Topology> topology = Topology::Resolve(gfx, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		AddBindable(topology);

		std::shared_ptr<RootSignature> rootSignature = std::make_shared<RootSignature>(gfx, "StandardSRVFullStage");
		AddBindable(rootSignature);

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		auto dsvFormat = DXGI_FORMAT_D32_FLOAT;

		AddBindable(std::make_shared<PipelineStateObject>(gfx, geometryTag + "PSO",
			rootSignature, vertexBuffer, topology, vertexShader, pixelShader, dsvFormat, rtvFormats));

		std::shared_ptr<ConstantTransformBuffer> transformBuffer = std::make_shared<ConstantTransformBuffer>(gfx);
		transformBuffer->InitializeParentReference(*this);
		AddBindable(transformBuffer);
	}

	SimpleSphere::~SimpleSphere()
	{
	}

	void SimpleSphere::SetPos(DirectX::XMFLOAT3 inputPosition) noexcept
	{
		position = inputPosition;
	}

	void SimpleSphere::SetRotation(DirectX::XMFLOAT3 inputRotation) noexcept
	{
		pitch = inputRotation.x;
		yaw = inputRotation.y;
		roll = inputRotation.z;
	}
	
	dx::XMMATRIX SimpleSphere::GetTransformXM() const noexcept
	{
		return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			dx::XMMatrixTranslation(position.x, position.y, position.z);
	}

	void SimpleSphere::Bind(DEGraphics::Graphics& gfx) const noxnd
	{
		Drawable::Bind(gfx);

		gfx.GetGraphicsCommandList()->DrawIndexedInstanced(indexBuffer->GetCount(), 1, 0, 0, 0);
	}
}