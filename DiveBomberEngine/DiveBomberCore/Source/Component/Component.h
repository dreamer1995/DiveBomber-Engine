#include "..\Graphics\Resource\Resource.h"
#pragma once

namespace DiveBomber::DEComponent
{
	class Component : public DEResource::Resource
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