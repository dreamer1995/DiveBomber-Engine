#pragma once
#include <string>
#include <vector>
#include <memory>

namespace DiveBomber::DEGraphic
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
		Drawable(DEGraphic::Graphics& gfx, const std::wstring inputName);
		~Drawable();

		virtual void Submit() = 0;
	private:
		const std::wstring name;
		std::vector<std::shared_ptr<BindableObject::Bindable>> bindableObjects;
	};
}