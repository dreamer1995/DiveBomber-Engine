#pragma once
#include "Drawable.h"

namespace DiveBomber::DrawableObject
{
	class Model final : public Drawable
	{
	public:
		Model();
		[[nodiscard]] int GetModel() const noexcept;
		void Bind() noxnd;
	private:
	};
}