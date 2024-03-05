#include "SimpleSphere.h"

#include "..\Graphics.h"
#include "..\Resource\GraphicsResourceCommonInclude.h"
#include "..\Geometry\Sphere.h"
#include "..\..\Component\Mesh.h"
#include "..\..\Component\Material.h"
#include "..\DX\CommandQueue.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\ShaderManager.h"
#include "..\DX\GlobalResourceManager.h"
#include "..\..\Utility\GlobalParameters.h"

namespace DiveBomber::DEObject
{
	using namespace DEGraphics;
	using namespace DiveBomber::DEResource;
	using namespace DiveBomber::DEResource::VertexProcess;
	using namespace DX;
	using namespace DEComponent;

	SimpleSphere::SimpleSphere(const std::wstring inputName)
		:
		Object(inputName)
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

		//std::shared_ptr<VertexBuffer> vertexBuffer = VertexBuffer::Resolve(geometryTag, sphere.vertices);
		std::shared_ptr<IndexBuffer> indexBuffer = GlobalResourceManager::Resolve<IndexBuffer>(name, sphere.indices);

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(name, sphere.vertices, indexBuffer);
		meshMap.emplace(mesh->GetName(), mesh);
		attachedComponents.emplace_back(mesh);

		std::shared_ptr<Material> material = GlobalResourceManager::Resolve<Material>(ProjectDirectoryW L"Asset\\Material\\" + name + L"Material", EngineShaderDirectoryW L"PBR");
		materialMap.emplace(material->GetName(), material);
		attachedComponents.emplace_back(material);

		std::shared_ptr<ConstantTransformBuffer> transformBuffer = std::make_shared<ConstantTransformBuffer>(name + L"Transforms");
		transformBuffer->InitializeParentReference(*this);
		AddBindable(transformBuffer);

		std::shared_ptr<RootSignature> rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 5;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		rtvFormats.RTFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvFormats.RTFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvFormats.RTFormats[3] = DXGI_FORMAT_R10G10B10A2_UNORM;
		rtvFormats.RTFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;

		auto dsvFormat = DXGI_FORMAT_D32_FLOAT;

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.mesh = mesh;
		pipelineStateReference.material = material;
		pipelineStateReference.rtvFormats = rtvFormats;
		pipelineStateReference.dsvFormat = dsvFormat;

		std::shared_ptr<PipelineStateObject> pipelineStateObject = GlobalResourceManager::Resolve<PipelineStateObject>(name, std::move(pipelineStateReference));
		AddBindable(pipelineStateObject);
	}

	SimpleSphere::~SimpleSphere()
	{
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

	std::unordered_map<std::wstring, std::shared_ptr<DEComponent::Material>> SimpleSphere::GetMaterials() const noexcept
	{
		return materialMap;
	}

	void SimpleSphere::Render() const noxnd
	{
		meshMap.begin()->second->Bind();
		materialMap.begin()->second->Bind();

		Object::Render();

		Graphics::GetInstance().GetGraphicsCommandList()->DrawIndexedInstanced(meshMap.begin()->second->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}
}