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
	class Drawable
	{
	public:
		Drawable() = default;
		Drawable(const std::wstring inputName);
		virtual ~Drawable();

		[[nodiscard]] virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

		void Submit() const noexcept;
		virtual void Bind() const noxnd;
		void AddBindable(std::shared_ptr<DEResource::Bindable> bindableObject) noexcept;

		std::wstring GetName() const noexcept;
	protected:
		std::wstring name;
		std::vector<std::shared_ptr<DEResource::Bindable>> bindableObjects;
	};
}