#include "SimpleSphere.h"

#include "..\Graphics.h"
#include "..\BindableObject\BindableObjectCommon.h"
#include "..\BindableObject\Geometry\Sphere.h"
#include "..\BindableObject\DynamicConstantBufferInHeap.h"
#include "..\BindableObject\DynamicStructuredBufferInHeap.h"
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

		using namespace std::string_literals;
		DynamicConstantProcess::RawLayout DCBLayout;
		DCBLayout.Add<DynamicConstantProcess::Array>("VertexData");
		DCBLayout["VertexData"].Set<DynamicConstantProcess::Struct>(8);

		DCBLayout["VertexData"].T().Add<DynamicConstantProcess::Float3>("Position"s);
		DCBLayout["VertexData"].T().Add<DynamicConstantProcess::Float2>("Texture2D"s);

		DynamicConstantProcess::Buffer DXBBuffer = DynamicConstantProcess::Buffer(std::move(DCBLayout));

		DXBBuffer["VertexData"][0]["Position"s] = dx::XMFLOAT3{ -1.0f, -1.0f, -1.0f };
		DXBBuffer["VertexData"][1]["Position"s] = dx::XMFLOAT3{ -1.0f, 1.0f, -1.0f };
		DXBBuffer["VertexData"][2]["Position"s] = dx::XMFLOAT3{ 1.0f,  1.0f, -1.0f };
		DXBBuffer["VertexData"][3]["Position"s] = dx::XMFLOAT3{ 1.0f, -1.0f, -1.0f };
		DXBBuffer["VertexData"][4]["Position"s] = dx::XMFLOAT3{ -1.0f, -1.0f,  1.0f };
		DXBBuffer["VertexData"][5]["Position"s] = dx::XMFLOAT3{ -1.0f,  1.0f,  1.0f };
		DXBBuffer["VertexData"][6]["Position"s] = dx::XMFLOAT3{ 1.0f,  1.0f,  1.0f };
		DXBBuffer["VertexData"][7]["Position"s] = dx::XMFLOAT3{ 1.0f, -1.0f,  1.0f };

		DXBBuffer["VertexData"][0]["Texture2D"s] = dx::XMFLOAT2{ 0.f, 0.f };
		DXBBuffer["VertexData"][1]["Texture2D"s] = dx::XMFLOAT2{ 0.f, 1.f };
		DXBBuffer["VertexData"][2]["Texture2D"s] = dx::XMFLOAT2{ 1.f, 1.f };
		DXBBuffer["VertexData"][3]["Texture2D"s] = dx::XMFLOAT2{ 1.f, 0.f };
		DXBBuffer["VertexData"][4]["Texture2D"s] = dx::XMFLOAT2{ 0.f, 1.f };
		DXBBuffer["VertexData"][5]["Texture2D"s] = dx::XMFLOAT2{ 0.f, 0.f };
		DXBBuffer["VertexData"][6]["Texture2D"s] = dx::XMFLOAT2{ 1.f, 0.f };
		DXBBuffer["VertexData"][7]["Texture2D"s] = dx::XMFLOAT2{ 1.f, 1.f };

		std::vector<unsigned short> indices = 
		{
				0, 1, 2, 0, 2, 3,
				4, 6, 5, 4, 7, 6,
				4, 5, 1, 4, 1, 0,
				3, 2, 6, 3, 6, 7,
				1, 5, 6, 1, 6, 2,
				4, 0, 3, 4, 3, 7
		};

		IndexedTriangleList sphere = Sphere::MakeNormalUVed(vl, true);
		sphere.Transform(dx::XMMatrixScaling(1, 1, 1));

		const std::string geometryTag = Utility::ToNarrow(name);

		std::shared_ptr<VertexBuffer> vertexBuffer = VertexBuffer::Resolve(geometryTag, sphere.vertices);
		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Resolve(geometryTag, indices);
		
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(name, vertexBuffer, indexBuffer);
		meshMap.emplace(mesh->GetName(), mesh);

		std::shared_ptr<Material> material = std::make_shared<Material>(name + L"Material");
		materialMap.emplace(material->GetName(), material);

		std::shared_ptr<ConstantTransformBuffer> transformBuffer = std::make_shared<ConstantTransformBuffer>();
		transformBuffer->InitializeParentReference(*this);
		AddBindable(transformBuffer);
		material->SetConstant(transformBuffer->GetTransformBuffer());

		std::shared_ptr<DynamicStructuredBufferInHeap> baseMat = std::make_shared<DynamicStructuredBufferInHeap>(Utility::ToNarrow(name), DXBBuffer);
		material->SetConstant(Utility::ToNarrow(name) + "VertexData", baseMat);

		std::shared_ptr<RootSignature> rootSignature = RootSignature::Resolve("StandardFullStageAccess");

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

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