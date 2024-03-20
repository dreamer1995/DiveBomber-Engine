#include "SkyDomePass.h"

#include "..\..\Graphics.h"
#include "..\..\GraphicResource\GraphicsResourceCommonInclude.h"
#include "..\..\Geometry\Cube.h"
#include "..\..\..\Component\Mesh.h"
#include "..\..\..\Component\Material.h"
#include "..\..\..\Component\Camera\Camera.h"
#include "..\..\DX\GlobalResourceManager.h"
#include "..\..\Object\Object.h"
#include "..\..\..\Utility\GlobalParameters.h"

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace GraphicResource;
	using namespace GraphicResource::VertexProcess;
	using namespace DX;
	using namespace DEComponent;
	using namespace DEObject;
	namespace dx = DirectX;

	SkyDomePass::SkyDomePass(std::shared_ptr<RenderTarget> inputRenderTarget,
		std::shared_ptr<GraphicResource::DepthStencil> inputDepthStencil)
		:
		RenderPass("SkyDome", inputRenderTarget, inputDepthStencil)
	{
		auto wName = Utility::ToWide(name);

		IndexedTriangleList cube = Cube::MakeIndependentTBN();
		cube.Transform(dx::XMMatrixScaling(2, 2, 2));
		cube.SetTBNIndependentFlat();

		std::shared_ptr<IndexBuffer> indexBuffer = GlobalResourceManager::Resolve<IndexBuffer>(wName, cube.indices);

		mesh = std::make_shared<Mesh>(wName, cube.vertices, indexBuffer);

		material = GlobalResourceManager::Resolve<Material>(EngineMaterialDirectoryW + wName + L"Material", EngineShaderDirectoryW L"SkyDome.hlsl");

		std::shared_ptr<RootSignature> rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		auto dsvFormat = DXGI_FORMAT_D32_FLOAT;

		CD3DX12_RASTERIZER_DESC rasterizerDesc{ D3D12_DEFAULT };
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc{ D3D12_DEFAULT };
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.mesh = mesh;
		pipelineStateReference.material = material;
		pipelineStateReference.rtvFormats = rtvFormats;
		pipelineStateReference.dsvFormat = dsvFormat;
		pipelineStateReference.rasterizerDesc = rasterizerDesc;
		pipelineStateReference.depthStencilDesc = depthStencilDesc;

		pso = GlobalResourceManager::Resolve<PipelineStateObject>(wName, std::move(pipelineStateReference));

		skyDomeAnchor = std::make_shared<Object>(wName);

		std::shared_ptr<GraphicResource::ConstantTransformBuffer> transformBuffer = std::make_shared<ConstantTransformBuffer>(wName + L"Transforms");
		transformBuffer->InitializeParentReference(*skyDomeAnchor.get());

		skyDomeAnchor->AddBindable(transformBuffer);
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
		skyDomeAnchor->SetPos(Graphics::GetInstance().GetCamera()->GetPos());
		skyDomeAnchor->Render();

		Graphics::GetInstance().GetGraphicsCommandList()->DrawIndexedInstanced(mesh->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}
}