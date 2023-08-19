#pragma once
#include "Bindable.h"

#include <DirectXMath.h>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::BindableObject
{
	template<typename C>
	class ConstantBuffer;

	class ConstantTransformBuffer final : Bindable
	{
	public:
		struct Transforms
		{
			DirectX::XMMATRIX matrix_MVP;
			DirectX::XMMATRIX matrix_MV;
			DirectX::XMMATRIX matrix_V;
			DirectX::XMMATRIX matrix_P;
			DirectX::XMMATRIX matrix_VP;
			DirectX::XMMATRIX matrix_T_MV;
			DirectX::XMMATRIX matrix_IT_MV;
			DirectX::XMMATRIX matrix_M2W;
			DirectX::XMMATRIX matrix_W2M;
			DirectX::XMMATRIX matrix_I_V;
			DirectX::XMMATRIX matrix_I_P;
			DirectX::XMMATRIX matrix_I_VP;
			DirectX::XMMATRIX matrix_I_MVP;
		};
	public:
		ConstantTransformBuffer(DEGraphics::Graphics& gfx);
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		[[nodiscard]] Transforms GetTransformMatrices() const noexcept;
		[[nodiscard]] Transforms CalculateTransformMatrices(const DEGraphics::Graphics& gfx) noxnd;
		void InitializeParentReference(DirectX::XMMATRIX inputModelMatrix) noexcept;


		//Maybe Transform info should be unique, or some of the matrices should be unique.
		//[[nodiscard]] static std::shared_ptr<ConstantTransformBuffer> Resolve(DEGraphics::Graphics& gfx);
		//[[nodiscard]] static std::string GenerateUID();
		//[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		Transforms transforms;
		std::unique_ptr<ConstantBuffer<Transforms>> transformCBuffer;
		DirectX::XMMATRIX modelMatrix;
	};
}