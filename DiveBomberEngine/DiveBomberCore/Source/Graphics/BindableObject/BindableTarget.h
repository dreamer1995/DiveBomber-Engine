#pragma once

namespace DiveBomber::BindableObject
{
	class BindableTarget
	{
	public:
		virtual ~BindableTarget() = default;
		virtual void BindTarget() noxnd = 0;
		virtual void BindTarget(std::shared_ptr<BindableTarget>) noxnd = 0;
	};
}
