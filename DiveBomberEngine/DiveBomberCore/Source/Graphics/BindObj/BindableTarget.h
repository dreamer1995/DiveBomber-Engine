#pragma once
#include "..\Graphics.h"

namespace DiveBomber::BindObj
{
	class BindableTarget
	{
	public:
		virtual ~BindableTarget() = default;
		virtual void BindTarget(DEGraphics::Graphics& gfx) noxnd = 0;
		virtual void BindTarget(DEGraphics::Graphics& gfx, std::shared_ptr<BindableTarget>) noxnd = 0;
	};
}
