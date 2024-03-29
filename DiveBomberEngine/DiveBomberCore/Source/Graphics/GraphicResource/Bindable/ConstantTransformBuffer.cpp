#include "ConstantTransformBuffer.h"

#include "ConstantBufferInRootSignature.h"
#include "..\..\Object\Object.h"
#include "..\..\..\Component\Camera\Camera.h"

namespace DiveBomber::GraphicResource
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DEObject;
	using namespace DEComponent;

	ConstantTransformBuffer::ConstantTransformBuffer(std::wstring inputName)
		:
		Resource(inputName)
	{
		using namespace std::string_literals;
		transformCBuffer = std::make_shared<ConstantBufferInRootSignature<Transforms>>(name + L"#"s + L"Transform Metrices", 3u);
	}

	void ConstantTransformBuffer::Bind() noxnd
	{
		transformCBuffer->Update(CalculateTransformMatrices());
		transformCBuffer->Bind();
	}

	ConstantTransformBuffer::Transforms ConstantTransformBuffer::GetTransformMatrices() const noexcept
	{
		return transforms;
	}

	void ConstantTransformBuffer::InitializeParentReference(const Object& inputParent) noexcept
	{
		parent = &inputParent;
	}

	std::shared_ptr<ConstantBufferInRootSignature<ConstantTransformBuffer::Transforms>> ConstantTransformBuffer::GetTransformBuffer() const noexcept
	{
		return transformCBuffer;
	}

	ConstantTransformBuffer::Transforms ConstantTransformBuffer::CalculateTransformMatrices() noxnd
	{
		const auto modelMatrix = parent != nullptr ? parent->GetTransformXM() : dx::XMMATRIX();
		const auto cameraTransforms = Graphics::GetInstance().GetCamera()->GetTransforms();

		const auto matrix_M2W = DirectX::XMMatrixTranspose(modelMatrix);
		const auto matrix_W2M = DirectX::XMMatrixInverse(nullptr, matrix_M2W);
		const auto matrix_V = cameraTransforms.matrix_V;
		const auto matrix_MV = matrix_V * matrix_M2W;
		const auto matrix_VP = cameraTransforms.matrix_VP;
		const auto matrix_P = cameraTransforms.matrix_P;
		const auto matrix_MVP = matrix_P * matrix_MV;
		const auto matrix_T_MV = DirectX::XMMatrixTranspose(matrix_MV);
		const auto matrix_IT_MV = DirectX::XMMatrixInverse(nullptr, matrix_T_MV);
		const auto matrix_I_MVP = DirectX::XMMatrixInverse(nullptr, matrix_MVP);
		return { matrix_MVP,
				matrix_MV,
				matrix_T_MV,
				matrix_IT_MV,
				matrix_M2W,
				matrix_W2M,
				matrix_I_MVP
		};
	}
}
