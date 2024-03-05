#pragma once
#include "Object.h"

namespace DiveBomber::DEObject
{
	class Model final : public Object
	{
	public:
		Model(const std::wstring inputName);
		[[nodiscard]] int GetModel() const noexcept;
		void Bind() noxnd;
	private:
	};
}