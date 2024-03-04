#include "FullScreenPlane.h"

#include "..\..\Graphics.h"
#include "..\..\Resource\ResourceCommonInclude.h"
#include "..\..\Geometry\Vertex.h"
#include "..\..\..\Component\Mesh.h"
#include "..\..\..\Component\Material.h"
#include "..\..\DX\GlobalResourceManager.h"
#include "..\..\..\Utility\GlobalParameters.h"

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DEResource;
	using namespace DEResource::VertexProcess;
	using namespace DX;
	using namespace DEComponent;
	namespace dx = DirectX;

	FullScreenPlane::FullScreenPlane(std::string inputName, std::shared_ptr<RenderTarget> inputRenderTarget)
		:
		RenderPass(inputName, inputRenderTarget, nullptr)
	{
		VertexProcess::VertexLayout lay;
		lay.Append(VertexProcess::VertexLayout::Position2D);

		VertexProcess::VertexData bufFull{ lay };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,-1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,-1 });

		std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
		std::shared_ptr<IndexBuffer> indexBuffer = GlobalResourceManager::Resolve<IndexBuffer>(L"FullScreenPlane", indices);
		
		mesh = std::make_shared<Mesh>(L"FullScreenPlane", bufFull, indexBuffer);

		material = GlobalResourceManager::Resolve<Material>(EngineMaterialDirectoryW L"FullScreenPlaneMaterial", EngineShaderDirectoryW L"FullScreen");

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

		pso = GlobalResourceManager::Resolve<PipelineStateObject>(L"FullScreenPlane", std::move(pipelineStateReference));
	}

	void FullScreenPlane::Execute() noxnd
	{
		for (auto& inputTexture : inputTexturesMap)
		{
			inputTexture.second->BindAsShaderResource();
			material->SetTexture(inputTexture.second, inputTexture.first);
		}

		for (auto& inputConstant : inputConstantsMap)
		{
			inputConstant.second->BindAsShaderResource();
			material->SetConstant(inputConstant.second, inputConstant.first);
		}

		RenderPass::Execute();

		mesh->Bind();
		material->Bind();
		pso->Bind();

		Graphics::GetInstance().GetGraphicsCommandList()->DrawIndexedInstanced(mesh->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}
}