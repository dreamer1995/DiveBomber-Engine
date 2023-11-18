#pragma once
#include "..\..\..\Utility\Common.h"

namespace DiveBomber::DEResource
{
	class Bindable
	{
	public:
		virtual ~Bindable() = default;

		virtual void Bind() noxnd = 0;
	};
}
