#include "Drawable.h"

#include "..\Graphics.h"
#include "..\BindableObject\Bindable.h"

namespace DiveBomber::DrawableObject
{
	using namespace DEGraphics;

	Drawable::Drawable(Graphics& gfx, const std::wstring inputName)
		:
		name(inputName)
	{
	}

	Drawable::~Drawable()
	{
	}

	void Drawable::Bind(Graphics& gfx) const noxnd
	{
		for (auto& bindableObject : bindableObjects)
		{
			bindableObject->Bind(gfx);
		}
	}

	void Drawable::AddBindable(std::shared_ptr<BindableObject::Bindable> bindableObject) noexcept
	{
		bindableObjects.emplace_back(bindableObject);
	}
}