#include "SkyDomePass.h"

#include "..\..\Graphics.h"
#include "..\..\Resource\ResourceCommonInclude.h"
#include "..\..\Geometry\Cube.h"
#include "..\..\Component\Mesh.h"
#include "..\..\Component\Material.h"
#include "..\..\DX\GlobalResourceManager.h"

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DEResource;
	using namespace DEResource::VertexProcess;
	using namespace DX;
	using namespace Component;
	namespace dx = DirectX;

	SkyDomePass::SkyDomePass(std::shared_ptr<RenderTarget> inputRenderTarget)
		:
		RenderPass("SkyDome", inputRenderTarget, nullptr)
	{
		auto wName = Utility::
		auto model = Cube::MakeIndependentTBN();
		model.SetTBNIndependentFlat();

		IndexedTriangleList cube = Cube::MakeIndependentTBN();
		cube.Transform(dx::XMMatrixScaling(1, 1, 1));
		cube.SetTBNIndependentFlat();

		std::shared_ptr<IndexBuffer> indexBuffer = GlobalResourceManager::Resolve<IndexBuffer>(name, cube.indices);

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(name, cube.vertices, indexBuffer);
		meshMap.emplace(mesh->GetName(), mesh);
		
		mesh = std::make_shared<Mesh>(L"SkyDome", bufFull, indexBuffer);

		material = std::make_shared<Material>(L"FullScreenPlaneMaterial", L"FullScreen");

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

	void SkyDomePass::Execute() noxnd
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