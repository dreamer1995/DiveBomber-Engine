#include "..\Resource.h"
#pragma once

namespace DiveBomber::DEComponent
{
	class Component : public Resource
	{
	public:
		Component(std::wstring inputName)
			:
			Resource(inputName)
		{
		}

		virtual void DrawComponentUI() = 0;
	};
}