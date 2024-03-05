#pragma once
#include "..\..\..\Utility\Common.h"

namespace DiveBomber::GraphicResource
{
	class Bindable
	{
	public:
		virtual ~Bindable() = default;

		virtual void Bind() noxnd = 0;
	};
}
