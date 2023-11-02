#include "SimpleSphere.h"

#include "..\Graphics.h"
#include "..\BindableObject\BindableObjectCommon.h"
#include "..\BindableObject\Geometry\Sphere.h"
#include "..\Component\Mesh.h"
#include "..\Component\Material.h"
#include "..\DX\CommandQueue.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\ShaderManager.h"

namespace DiveBomber::DrawableObject
{
	using namespace DEGraphics;
	using namespace DiveBomber::BindableObject;
	using namespace DiveBomber::BindableObject::VertexProcess;
	using namespace DX;
	using namespace Component;
	namespace dx = DirectX;

	SimpleSphere::SimpleSphere(const std::wstring inputName)
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

		//std::shared_ptr<VertexBuffer> vertexBuffer = VertexBuffer::Resolve(geometryTag, sphere.vertices);
		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Resolve(geometryTag, sphere.indices);
		
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(name, sphere.vertices, indexBuffer);
		meshMap.emplace(mesh->GetName(), mesh);

		std::shared_ptr<Material> material = std::make_shared<Material>(name + L"Material");
		materialMap.emplace(material->GetName(), material);

		std::shared_ptr<RootSignature> rootSignature = RootSignature::Resolve("StandardFullStageAccess");
		AddBindable(rootSignature);

		std::shared_ptr<ConstantTransformBuffer> transformBuffer = std::make_shared<ConstantTransformBuffer>(name + L"Transforms");
		transformBuffer->InitializeParentReference(*this);
		AddBindable(transformBuffer);

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		auto dsvFormat = DXGI_FORMAT_D32_FLOAT;

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.mesh = mesh;
		pipelineStateReference.material = material;
		pipelineStateReference.rtvFormats = rtvFormats;
		pipelineStateReference.dsvFormat = dsvFormat;

		std::shared_ptr<PipelineStateObject> pipelineStateObject = PipelineStateObject::Resolve(geometryTag, std::move(pipelineStateReference));
		PSOMap.emplace(pipelineStateObject->GetUID(), pipelineStateObject);
	}

	SimpleSphere::~SimpleSphere()
	{
	}

	void SimpleSphere::SetPos(dx::XMFLOAT3 inputPosition) noexcept
	{
		position = inputPosition;
	}

	void SimpleSphere::SetRotation(dx::XMFLOAT3 inputRotation) noexcept
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

	std::shared_ptr<Material> SimpleSphere::GetMaterialByName(std::wstring name) const noexcept
	{
		auto it = materialMap.find(name);
		if (it != materialMap.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}

	std::unordered_map<std::wstring, std::shared_ptr<Component::Material>> SimpleSphere::GetMaterials() const noexcept
	{
		return materialMap;
	}

	void SimpleSphere::Bind() const noxnd
	{
		Drawable::Bind();
		for (auto& pso : PSOMap)
		{
			pso.second->Bind();
		}
	}
}