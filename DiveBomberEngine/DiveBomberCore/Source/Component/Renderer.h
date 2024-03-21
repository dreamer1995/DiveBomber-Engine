#pragma once
#include "Component.h"
#include "..\Graphics\GraphicsHeader.h"
#include "..\ConfigDrivenResource.h"

namespace DiveBomber::DEComponent
{
	class Renderer final : public Component, public DiveBomber::ConfigDrivenResource
	{
	public:
		Renderer(const std::wstring inputName);
	private:
	};
}