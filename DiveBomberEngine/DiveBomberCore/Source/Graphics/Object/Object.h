#pragma once
#include "..\..\Utility\Common.h"
#include "..\..\ConfigDrivenResource.h"
#include "..\..\Resource.h"

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <memory>

namespace DiveBomber::GraphicResource
{
	class Bindable;
}

namespace DiveBomber::DEComponent
{
	class Component;
}

namespace DiveBomber::DEObject
{
	class Object : public DiveBomber::Resource, public DiveBomber::ConfigDrivenResource
	{
	public:
		Object(const std::wstring inputName);
		virtual ~Object();

		void SetPos(DirectX::XMFLOAT3 inputPosition = { 0.0f,0.0f,0.0f }) noexcept;
		[[nodiscard]] DirectX::XMFLOAT3 GetPos(DirectX::XMFLOAT3 inputPosition = { 0.0f,0.0f,0.0f }) noexcept;
		void SetRotation(DirectX::XMFLOAT3 inputRotation = { 0.0f,0.0f,0.0f }) noexcept;
		[[nodiscard]] DirectX::XMFLOAT3 GetRotation(DirectX::XMFLOAT3 inputRotation = { 0.0f,0.0f,0.0f }) noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetTransformXM() const noexcept;

		virtual void Render() const noxnd;
		void AddBindable(std::shared_ptr<GraphicResource::Bindable> bindableObject) noexcept;

		void virtual DrawDetailPanel() override;
		void virtual SaveConfig() override;
	protected:
		std::wstring name;

		DirectX::XMFLOAT3 position{ 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 rotation{ 0.0f,0.0f,0.0f };

		std::vector<std::shared_ptr<GraphicResource::Bindable>> bindableObjects;
		std::vector<std::shared_ptr<DEComponent::Component>> attachedComponents;
	};
}