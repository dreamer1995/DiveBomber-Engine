#include "FinalPostProcessPass.h"

#include "..\..\Graphics.h"
#include "..\..\Component\Material.h"

#include "..\..\Resource\ResourceCommonInclude.h"
#include "..\..\DX\GlobalResourceManager.h"

#include <iostream>

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DEResource;
	using namespace DX;
	using namespace Component;

	FinalPostProcessPass::FinalPostProcessPass(std::shared_ptr<UnorderedAccessBuffer> inputTarget)
		:
		ComputePass("FinalPostProcessPass", inputTarget)
	{
		material = std::make_shared<Material>(L"FinalPostProcessMaterial", L"PostProcess");

		std::shared_ptr<RootSignature> rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.material = material;

		pso = GlobalResourceManager::Resolve<PipelineStateObject>(L"FinalPostProcess", std::move(pipelineStateReference));
	}

	void FinalPostProcessPass::Execute() noxnd
	{
		ComputePass::Execute();

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

		material->Bind();
		pso->Bind();

		D3D12_RESOURCE_DESC uavdesc = uavTarget->GetUnorderedAccessBuffer()->GetDesc();

		Graphics::GetInstance().GetGraphicsCommandList()->Dispatch(
			((UINT)uavdesc.Width + 7u) / 8,
			((UINT)uavdesc.Height + 7u) / 8,
			1u);
	}
}
