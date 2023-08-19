#pragma once
#include <string>

namespace DiveBomber::DrawableObject
{
	class Drawable final
	{
	public:
		Drawable();
		~Drawable();
	private:
		const std::wstring name;
	};
}