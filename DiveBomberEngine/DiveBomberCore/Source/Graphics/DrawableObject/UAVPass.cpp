#include "UAVPass.h"

#include "..\Graphics.h"
#include "..\Component\Material.h"

#include "..\BindableObject\BindableObjectCommon.h"

namespace DiveBomber::DrawableObject
{
	using namespace DEGraphics;
	using namespace DiveBomber::BindableObject;
	using namespace Component;

	UAVPass::UAVPass(const std::wstring inputName)
	{
		name = inputName;

		material = std::make_shared<Material>(name + L"Material");

		std::shared_ptr<RootSignature> rootSignature = RootSignature::Resolve("StandardFullStageAccess");

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.material = material;

		pso = PipelineStateObject::Resolve(Utility::ToNarrow(name), std::move(pipelineStateReference));
	}

	UAVPass::~UAVPass()
	{
	}

	void UAVPass::SetTexture(const std::shared_ptr<BindableShaderInput> texture)
	{
		material->SetTexture(texture);
	}

	void UAVPass::Bind() const noxnd
	{
		material->Bind();
		pso->Bind();

		Graphics::GetInstance().GetGraphicsCommandList()->Dispatch(
			(Graphics::GetInstance().GetWidth() + 7u) / 8,
			(Graphics::GetInstance().GetHeight() + 7u) / 8,
			1u);
	}
}