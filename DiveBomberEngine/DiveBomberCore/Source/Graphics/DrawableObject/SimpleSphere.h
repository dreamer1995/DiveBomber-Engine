#pragma once
#include "Drawable.h"

#include <unordered_map>

namespace DiveBomber::BindableObject
{
	class Bindable;
	class IndexBuffer;
	class PipelineStateObject;
}

namespace DiveBomber::Component
{
	class Mesh;
	class Material;
}

namespace DiveBomber::DrawableObject
{
	class SimpleSphere final : public Drawable
	{
	public:
		SimpleSphere(const std::wstring inputName = L"Sphere");
		~SimpleSphere();

		void Bind() const noxnd override;

		void SetPos(DirectX::XMFLOAT3 inputPosition = { 0.0f,0.0f,0.0f }) noexcept;
		void SetRotation(DirectX::XMFLOAT3 inputRotation = { 0.0f,0.0f,0.0f }) noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetTransformXM() const noexcept override;
		[[nodiscard]] std::shared_ptr<Component::Material> GetMaterialByName(std::wstring name) const noexcept;
	private:
		DirectX::XMFLOAT3 position{ 0.0f,0.0f,0.0f };
		float pitch{ 0.0f };
		float yaw{ 0.0f };
		float roll{ 0.0f };
		std::unordered_map<std::wstring, std::shared_ptr<Component::Mesh>> meshMap;
		std::unordered_map<std::wstring, std::shared_ptr<Component::Material>> materialMap;
		std::unordered_map<std::string, std::shared_ptr<BindableObject::PipelineStateObject>> PSOMap;
	};
}