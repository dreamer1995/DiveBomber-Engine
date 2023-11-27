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

namespace DiveBomber::DEObject
{
	class Object
	{
	public:
		Object() = default;
		Object(const std::wstring inputName);
		virtual ~Object();

		void SetPos(DirectX::XMFLOAT3 inputPosition = { 0.0f,0.0f,0.0f }) noexcept;
		void SetRotation(DirectX::XMFLOAT3 inputRotation = { 0.0f,0.0f,0.0f }) noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetTransformXM() const noexcept;

		virtual void Render() const noxnd;
		void AddBindable(std::shared_ptr<DEResource::Bindable> bindableObject) noexcept;

		std::wstring GetName() const noexcept;
	protected:
		std::wstring name;
		std::vector<std::shared_ptr<DEResource::Bindable>> bindableObjects;

		DirectX::XMFLOAT3 position{ 0.0f,0.0f,0.0f };
		float pitch{ 0.0f };
		float yaw{ 0.0f };
		float roll{ 0.0f };
	};
}