#include "UAVPass.h"

#include "..\Graphics.h"
#include "..\Component\Material.h"

#include "..\Resource\ResourceCommonInclude.h"
#include "..\DX\GlobalResourceManager.h"

namespace DiveBomber::DEObject
{
	using namespace DEGraphics;
	using namespace DiveBomber::DEResource;
	using namespace Component;
	using namespace DX;

	UAVPass::UAVPass(const std::wstring inputName)
	{
		name = inputName;

		material = std::make_shared<Material>(name + L"Material");

		std::shared_ptr<RootSignature> rootSignature = GlobalResourceManager::Resolve<RootSignature>(L"StandardFullStageAccess");

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.material = material;

		pso = GlobalResourceManager::Resolve<PipelineStateObject>(name, std::move(pipelineStateReference));
	}

	UAVPass::~UAVPass()
	{
	}

	void UAVPass::SetTexture(const std::shared_ptr<ShaderInputable> texture)
	{
		material->SetTexture(texture);
	}

	void UAVPass::Execute() const noxnd
	{
		material->Bind();
		pso->Bind();

		Graphics::GetInstance().GetGraphicsCommandList()->Dispatch(
			(Graphics::GetInstance().GetWidth() + 7u) / 8,
			(Graphics::GetInstance().GetHeight() + 7u) / 8,
			1u);
	}
}