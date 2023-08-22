#pragma once
#include "Drawable.h"

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::DrawableObject
{
	class Model final : public Drawable
	{
	public:
		Model();
		[[nodiscard]] int GetModel() const noexcept;
		void Bind(DEGraphics::Graphics& gfx) noxnd;
	private:
	};
}