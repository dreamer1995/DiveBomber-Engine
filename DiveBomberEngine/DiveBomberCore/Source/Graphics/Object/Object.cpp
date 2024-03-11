#include "Object.h"

#include "..\Graphics.h"
#include "..\GraphicResource\Bindable\Bindable.h"
#include "..\..\Component\Component.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::DEObject
{
	using namespace DEGraphics;
	using namespace DEComponent;

	Object::Object(const std::wstring inputName)
		:
		Resource(inputName),
		ConfigDrivenResource(inputName)
	{
	}

	Object::~Object()
	{
	}

	void Object::SetPos(dx::XMFLOAT3 inputPosition) noexcept
	{
		position = inputPosition;
	}

	DirectX::XMFLOAT3 Object::GetPos(DirectX::XMFLOAT3 inputPosition) noexcept
	{
		return position;
	}

	void Object::SetRotation(dx::XMFLOAT3 inputRotation) noexcept
	{
		rotation = inputRotation;
	}

	DirectX::XMFLOAT3 Object::GetRotation(DirectX::XMFLOAT3 inputRotation) noexcept
	{
		return rotation;
	}

	dx::XMMATRIX Object::GetTransformXM() const noexcept
	{
		return dx::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
			dx::XMMatrixTranslation(position.x, position.y, position.z);
	}

	void Object::Render() const noxnd
	{
		for (auto& bindableObject : bindableObjects)
		{
			bindableObject->Bind();
		}
	}

	void Object::AddBindable(std::shared_ptr<GraphicResource::Bindable> bindableObject) noexcept
	{
		bindableObjects.emplace_back(bindableObject);
	}

	void DiveBomber::DEObject::Object::DrawDetailPanel()
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f");
			ImGui::SliderFloat("Y", &position.y, -80.0f, 80.0f, "%.1f");
			ImGui::SliderFloat("Z", &position.z, -80.0f, 80.0f, "%.1f");
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Pitch", &rotation.x, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &rotation.y, -180.0f, 180.0f);
			ImGui::SliderAngle("Roll", &rotation.z, -180.0f, 180.0f);
		}

		for (std::shared_ptr<Component> component : attachedComponents)
		{
			component->DrawComponentUI();
		}
	}

	void DiveBomber::DEObject::Object::CreateConfig()
	{
	}

	void DiveBomber::DEObject::Object::SaveConfig()
	{
	}
}