#pragma once
#include "..\..\..\Resource.h"
#include "Bindable.h"

#include <DirectXMath.h>

namespace DiveBomber::DEObject
{
	class Object;
}

namespace DiveBomber::GraphicResource
{
	template<typename C>
	class ConstantBufferInRootSignature;

	class ConstantTransformBuffer final : public DiveBomber::Resource, public Bindable
	{
	public:
		struct alignas(16) Transforms
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
		void InitializeParentReference(const DEObject::Object& inputParent) noexcept;
		std::shared_ptr<ConstantBufferInRootSignature<Transforms>> GetTransformBuffer() const noexcept;
	private:
		Transforms transforms;
		std::shared_ptr<ConstantBufferInRootSignature<Transforms>> transformCBuffer;
		const DEObject::Object* parent = nullptr;
	};
}