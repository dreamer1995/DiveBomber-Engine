#include "ConstantTransformBuffer.h"

#include "BindableCodex.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	ConstantTransformBuffer::ConstantTransformBuffer(Graphics& gfx)
	{
		transformCBuffer = std::make_unique<ConstantBuffer<Transforms>>(gfx, "Transform Metrices", 0u);
	}

	void ConstantTransformBuffer::Bind(Graphics& gfx) noxnd
	{
		transformCBuffer->Update(gfx, CalculateTransformMatrices(gfx));
		transformCBuffer->Bind(gfx);
	}

	ConstantTransformBuffer::Transforms ConstantTransformBuffer::GetTransformMatrices() const noexcept
	{
		return transforms;
	}

	ConstantTransformBuffer::Transforms ConstantTransformBuffer::CalculateTransformMatrices(const Graphics& gfx) noxnd
	{
		const auto matrix_M2W = DirectX::XMMatrixTranspose(modelMatrix);
		const auto matrix_W2M = DirectX::XMMatrixInverse(nullptr, matrix_M2W);
		const auto matrix_V = DirectX::XMMatrixTranspose(gfx.GetCameraMatrix());
		const auto matrix_MV = matrix_V * matrix_M2W;
		const auto matrix_P = DirectX::XMMatrixTranspose(gfx.GetProjetionMatrix());
		const auto matrix_VP = matrix_P * matrix_V;
		const auto matrix_MVP = matrix_P * matrix_MV;
		const auto matrix_T_MV = modelMatrix * gfx.GetCameraMatrix();
		const auto matrix_IT_MV = DirectX::XMMatrixInverse(nullptr, matrix_T_MV);
		const auto matrix_I_V = DirectX::XMMatrixInverse(nullptr, matrix_V);
		const auto matrix_I_P = DirectX::XMMatrixInverse(nullptr, matrix_P);
		const auto matrix_I_VP = DirectX::XMMatrixInverse(nullptr, matrix_VP);
		const auto matrix_I_MVP = DirectX::XMMatrixInverse(nullptr, matrix_MVP);
		return { matrix_MVP,
				matrix_MV,
				matrix_V,
				matrix_P,
				matrix_VP,
				matrix_T_MV,
				matrix_IT_MV,
				matrix_M2W,
				matrix_W2M,
				matrix_I_V,
				matrix_I_P,
				matrix_I_VP,
				matrix_I_MVP
		};
	}

	void ConstantTransformBuffer::InitializeParentReference(DirectX::XMMATRIX inputModelMatrix) noexcept
	{
		modelMatrix = inputModelMatrix;
	}
}
