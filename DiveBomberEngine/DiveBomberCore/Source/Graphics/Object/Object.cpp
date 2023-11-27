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

	void Object::SetPos(dx::XMFLOAT3 inputPosition) noexcept
	{
		position = inputPosition;
	}

	void Object::SetRotation(dx::XMFLOAT3 inputRotation) noexcept
	{
		pitch = inputRotation.x;
		yaw = inputRotation.y;
		roll = inputRotation.z;
	}

	dx::XMMATRIX Object::GetTransformXM() const noexcept
	{
		return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			dx::XMMatrixTranslation(position.x, position.y, position.z);
	}

	void Object::Render() const noxnd
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