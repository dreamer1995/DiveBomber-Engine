#include "FullScreenPlane.h"

#include "..\Graphics.h"
#include "..\Resource\ResourceCommonInclude.h"
#include "..\Geometry\Vertex.h"
#include "..\Component\Mesh.h"
#include "..\Component\Material.h"
#include "..\DX\CommandQueue.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\ShaderManager.h"
#include "..\DX\GlobalResourceManager.h"

namespace DiveBomber::DEObject
{
	using namespace DiveBomber::DEGraphics;
	using namespace DiveBomber::DEResource;
	using namespace DiveBomber::DEResource::VertexProcess;
	using namespace DX;
	using namespace Component;
	namespace dx = DirectX;

	FullScreenPlane::FullScreenPlane(const std::wstring inputName)
	{
		name = inputName;

		VertexProcess::VertexLayout lay;
		lay.Append(VertexProcess::VertexLayout::Position2D);

		VertexProcess::VertexData bufFull{ lay };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,-1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,-1 });

		std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
		std::shared_ptr<IndexBuffer> indexBuffer = GlobalResourceManager::Resolve<IndexBuffer>(name, indices);
		
		mesh = std::make_shared<Mesh>(name, bufFull, indexBuffer);

		material = std::make_shared<Material>(name + L"Material");

		std::shared_ptr<RootSignature> rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		auto dsvFormat = DXGI_FORMAT_UNKNOWN;

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.mesh = mesh;
		pipelineStateReference.material = material;
		pipelineStateReference.rtvFormats = rtvFormats;
		pipelineStateReference.dsvFormat = dsvFormat;

		std::shared_ptr<PipelineStateObject> pipelineStateObject = GlobalResourceManager::Resolve<PipelineStateObject>(name, std::move(pipelineStateReference));
		AddBindable(pipelineStateObject);
	}

	FullScreenPlane::~FullScreenPlane()
	{
	}

	dx::XMMATRIX FullScreenPlane::GetTransformXM() const noexcept
	{
		return DirectX::XMMATRIX();
	}

	void FullScreenPlane::SetTexture(const std::shared_ptr<ShaderInputable> texture)
	{
		material->SetTexture(texture);
	}

	void FullScreenPlane::Bind() const noxnd
	{
		mesh->Bind();
		material->Bind();

		Object::Bind();

		Graphics::GetInstance().GetGraphicsCommandList()->DrawIndexedInstanced(mesh->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}
}