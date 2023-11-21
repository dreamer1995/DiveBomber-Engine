#include "Object.h"

#include "..\Graphics.h"
#include "..\Resource\Bindable\Bindable.h"

namespace DiveBomber::DEObject
{
	using namespace DEGraphics;

	Object::Object(const std::wstring inputName)
		:
		name(inputName)
	{
	}

	Object::~Object()
	{
	}

	void Object::Bind() const noxnd
	{
		for (auto& bindableObject : bindableObjects)
		{
			bindableObject->Bind();
		}
	}

	void Object::AddBindable(std::shared_ptr<DEResource::Bindable> bindableObject) noexcept
	{
		bindableObjects.emplace_back(bindableObject);
	}

	std::wstring Object::GetName() const noexcept
	{
		return name;
	}
}