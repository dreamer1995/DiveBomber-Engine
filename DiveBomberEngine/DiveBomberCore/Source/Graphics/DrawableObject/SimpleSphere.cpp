#include "SimpleSphere.h"

#include "..\Graphics.h"
#include "..\BindableObject\BindableObjectCommon.h"
#include "..\BindableObject\Geometry\Sphere.h"
#include "..\Component\Mesh.h"
#include "..\Component\Material.h"
#include "..\DX\CommandQueue.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

namespace DiveBomber::DrawableObject
{
	using namespace DEGraphics;
	using namespace DiveBomber::BindableObject;
	using namespace DiveBomber::BindableObject::VertexProcess;
	using namespace DX;
	using namespace Component;
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

		IndexedTriangleList sphere = Sphere::MakeNormalUVed(vl, true);
		sphere.Transform(dx::XMMatrixScaling(1, 1, 1));

		const std::string geometryTag = Utility::ToNarrow(name);

		std::shared_ptr<VertexBuffer> vertexBuffer = VertexBuffer::Resolve(gfx, geometryTag, sphere.vertices);
		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Resolve(gfx, geometryTag, sphere.indices);

		std::shared_ptr<Topology> topology = Topology::Resolve(gfx, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		
		mesh = std::make_shared<Mesh>(gfx, vertexBuffer, indexBuffer);

		material = std::make_shared<Material>(gfx);
		material->AddTexture(Texture::Resolve(gfx, L"earth.dds"), 0u);
		material->AddTexture(Texture::Resolve(gfx, L"rustediron2_basecolor.png"), 1u);

		std::shared_ptr<ConstantTransformBuffer> transformBuffer = std::make_shared<ConstantTransformBuffer>(gfx);
		transformBuffer->InitializeParentReference(*this);
		AddBindable(transformBuffer);
		material->AddConstant(transformBuffer->GetTransformBuffer(), 0u);

		std::shared_ptr<Shader> vertexShader = Shader::Resolve(gfx, L"VShader", Shader::ShaderType::VertexShader);
		AddBindable(vertexShader);
		std::shared_ptr<Shader> pixelShader = Shader::Resolve(gfx, L"PShader", Shader::ShaderType::PixelShader);
		AddBindable(pixelShader);

		std::shared_ptr<RootSignature> rootSignature = RootSignature::Resolve(gfx, "StandardSRVFullStage");
		AddBindable(rootSignature);

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		auto dsvFormat = DXGI_FORMAT_D32_FLOAT;

		AddBindable(PipelineStateObject::Resolve(gfx, geometryTag + "PSO",
			rootSignature, mesh->GetVertexBuffer(), mesh->GetTopology(), vertexShader, pixelShader, dsvFormat, rtvFormats));
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
		mesh->Bind(gfx);
		Drawable::Bind(gfx);
		material->Bind(gfx);

		gfx.GetGraphicsCommandList()->DrawIndexedInstanced(mesh->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}
}