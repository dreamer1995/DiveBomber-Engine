#pragma once
#include "..\..\Utility\Common.h"

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <memory>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::BindableObject
{
	class Bindable;
}

namespace DiveBomber::DrawableObject
{
	class Drawable
	{
	public:
		Drawable() = default;
		Drawable(DEGraphics::Graphics& gfx, const std::wstring inputName);
		virtual ~Drawable();

		[[nodiscard]] virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

		void Submit() const noexcept;
		virtual void Bind(DEGraphics::Graphics& gfx) const noxnd;
		void AddBindable(std::shared_ptr<BindableObject::Bindable> bindableObject) noexcept;

		std::wstring GetName() const noexcept;
	protected:
		std::wstring name;
		std::vector<std::shared_ptr<BindableObject::Bindable>> bindableObjects;
	};
}