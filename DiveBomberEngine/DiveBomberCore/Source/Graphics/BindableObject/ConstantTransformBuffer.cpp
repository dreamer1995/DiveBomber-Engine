#include "ConstantTransformBuffer.h"

#include "ConstantBufferInHeap.h"
#include "..\DrawableObject\Drawable.h"

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DrawableObject;

	ConstantTransformBuffer::ConstantTransformBuffer(Graphics& gfx)
	{
		transformCBuffer = std::make_shared<ConstantBufferInHeap<Transforms>>(gfx, "Transform Metrices");
	}

	void ConstantTransformBuffer::Bind(Graphics& gfx) noxnd
	{
		Transforms bindedTransform = CalculateTransformMatrices(gfx);
		transformCBuffer->Update(gfx, &bindedTransform, sizeof(bindedTransform));
	}

	ConstantTransformBuffer::Transforms ConstantTransformBuffer::GetTransformMatrices() const noexcept
	{
		return transforms;
	}

	void ConstantTransformBuffer::InitializeParentReference(const Drawable& inputParent) noexcept
	{
		parent = &inputParent;
	}

	std::shared_ptr<ConstantBufferInHeap<ConstantTransformBuffer::Transforms>> ConstantTransformBuffer::GetTransformBuffer() const noexcept
	{
		return transformCBuffer;
	}

	ConstantTransformBuffer::Transforms ConstantTransformBuffer::CalculateTransformMatrices(const Graphics& gfx) noxnd
	{
		const auto modelMatrix = parent->GetTransformXM();
		const auto cameraMatrix = gfx.GetCameraMatrix();
		const auto projectionMatrix = gfx.GetProjetionMatrix();

		const auto matrix_M2W = DirectX::XMMatrixTranspose(modelMatrix);
		const auto matrix_W2M = DirectX::XMMatrixInverse(nullptr, matrix_M2W);
		const auto matrix_V = DirectX::XMMatrixTranspose(cameraMatrix);
		const auto matrix_MV = matrix_V * matrix_M2W;
		const auto matrix_P = DirectX::XMMatrixTranspose(projectionMatrix);
		const auto matrix_VP = matrix_P * matrix_V;
		const auto matrix_MVP = matrix_P * matrix_MV;
		const auto matrix_T_MV = DirectX::XMMatrixTranspose(matrix_MV);
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
}
