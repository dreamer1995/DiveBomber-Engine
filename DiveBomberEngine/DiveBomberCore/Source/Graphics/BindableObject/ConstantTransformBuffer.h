#pragma once
#include "Bindable.h"

#include <DirectXMath.h>

namespace DiveBomber::DrawableObject
{
	class Drawable;
}

namespace DiveBomber::BindableObject
{
	template<typename C>
	class ConstantBuffer;

	class ConstantTransformBuffer final : public Bindable
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
		ConstantTransformBuffer(std::wstring inputName);
		void Bind() noxnd override;
		[[nodiscard]] Transforms GetTransformMatrices() const noexcept;
		[[nodiscard]] Transforms CalculateTransformMatrices() noxnd;
		void InitializeParentReference(const DrawableObject::Drawable& inputParent) noexcept;
		std::shared_ptr<ConstantBuffer<Transforms>> GetTransformBuffer() const noexcept;


		//Maybe Transform info should be unique, or some of the matrices should be unique.
		//[[nodiscard]] static std::shared_ptr<ConstantTransformBuffer> Resolve(DEGraphics::Graphics& gfx);
		//[[nodiscard]] static std::string GenerateUID();
		//[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		std::wstring name;
		Transforms transforms;
		std::shared_ptr<ConstantBuffer<Transforms>> transformCBuffer;
		const DrawableObject::Drawable* parent = nullptr;
	};
}