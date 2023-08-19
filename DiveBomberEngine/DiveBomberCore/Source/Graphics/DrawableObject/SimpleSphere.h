#pragma once
#include "Drawable.h"

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::BindableObject
{
	class Bindable;
	class IndexBuffer;
}

namespace DiveBomber::DrawableObject
{
	class SimpleSphere final : public Drawable
	{
	public:
		SimpleSphere(DEGraphics::Graphics& gfx, const std::wstring inputName = L"Sphere");
		~SimpleSphere();

		void Bind(DEGraphics::Graphics& gfx) const noxnd override;

		void SetPos(DirectX::XMFLOAT3 inputPosition = { 0.0f,0.0f,0.0f }) noexcept;
		void SetRotation(DirectX::XMFLOAT3 inputRotation = { 0.0f,0.0f,0.0f }) noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetTransformXM() const noexcept override;
	private:
		DirectX::XMFLOAT3 position{ 0.0f,0.0f,0.0f };
		float pitch{ 0.0f };
		float yaw{ 0.0f };
		float roll{ 0.0f };
		std::shared_ptr<BindableObject::IndexBuffer> indexBuffer;
	};
}