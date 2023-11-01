#include "FullScreenPlane.h"

#include "..\Graphics.h"
#include "..\BindableObject\BindableObjectCommon.h"
#include "..\BindableObject\Vertex.h"
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

		const std::string geometryTag = Utility::ToNarrow(name);

		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Resolve(geometryTag, { 0,1,2,1,3,2 });
		
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(name, bufFull, indexBuffer);

		material = std::make_shared<Material>(name + L"Material");

		std::shared_ptr<RootSignature> rootSignature = RootSignature::Resolve("StandardFullStageAccess");
		AddBindable(rootSignature);

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
		AddBindable(pipelineStateObject);
	}

	FullScreenPlane::~FullScreenPlane()
	{
	}

	dx::XMMATRIX FullScreenPlane::GetTransformXM() const noexcept
	{
		return DirectX::XMMATRIX();
	}

	void FullScreenPlane::Bind() const noxnd
	{
		Drawable::Bind();
		material->Bind();
	}
}