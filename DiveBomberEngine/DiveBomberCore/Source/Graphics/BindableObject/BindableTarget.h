#pragma once
namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::BindableObject
{
	class BindableTarget
	{
	public:
		virtual ~BindableTarget() = default;
		virtual void BindTarget(DEGraphics::Graphics& gfx) noxnd = 0;
		virtual void BindTarget(DEGraphics::Graphics& gfx, std::shared_ptr<BindableTarget>) noxnd = 0;
	};
}
