#include "Drawable.h"

#include "..\Graphics.h"
#include "..\BindableObject\Bindable.h"

namespace DiveBomber::DrawableObject
{
	using namespace DEGraphics;

	Drawable::Drawable(const std::wstring inputName)
		:
		name(inputName)
	{
	}

	Drawable::~Drawable()
	{
	}

	void Drawable::Bind() const noxnd
	{
		for (auto& bindableObject : bindableObjects)
		{
			bindableObject->Bind();
		}
	}

	void Drawable::AddBindable(std::shared_ptr<BindableObject::Bindable> bindableObject) noexcept
	{
		bindableObjects.emplace_back(bindableObject);
	}

	std::wstring Drawable::GetName() const noexcept
	{
		return name;
	}
}