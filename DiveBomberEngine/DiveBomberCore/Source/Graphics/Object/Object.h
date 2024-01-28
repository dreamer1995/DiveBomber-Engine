#pragma once
#include "..\..\Utility\Common.h"

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <memory>

namespace DiveBomber::DEResource
{
	class Bindable;
}

namespace DiveBomber::DEComponent
{
	class Component;
}

namespace DiveBomber::DEObject
{
	class Object
	{
	public:
		Object() = default;
		Object(const std::wstring inputName);
		virtual ~Object();

		void SetPos(DirectX::XMFLOAT3 inputPosition = { 0.0f,0.0f,0.0f }) noexcept;
		[[nodiscard]] DirectX::XMFLOAT3 GetPos(DirectX::XMFLOAT3 inputPosition = { 0.0f,0.0f,0.0f }) noexcept;
		void SetRotation(DirectX::XMFLOAT3 inputRotation = { 0.0f,0.0f,0.0f }) noexcept;
		[[nodiscard]] DirectX::XMFLOAT3 GetRotation(DirectX::XMFLOAT3 inputRotation = { 0.0f,0.0f,0.0f }) noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetTransformXM() const noexcept;

		virtual void Render() const noxnd;
		void AddBindable(std::shared_ptr<DEResource::Bindable> bindableObject) noexcept;

		[[nodiscard]] std::wstring GetName() const noexcept;

		virtual void DrawDetailPanel();
	protected:
		std::wstring name;

		DirectX::XMFLOAT3 position{ 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 rotation{ 0.0f,0.0f,0.0f };

		std::vector<std::shared_ptr<DEResource::Bindable>> bindableObjects;
		std::vector<std::shared_ptr<DEComponent::Component>> attachedComponents;
	};
}