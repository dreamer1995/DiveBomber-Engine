#pragma once
#include "..\..\Utility\Common.h"

#include <memory>
#include <cassert>

namespace DiveBomber::BindableObject
{
	class Bindable
	{
	public:
		virtual void Bind() noxnd = 0;
		//virtual void InitializeParentReference(const Drawable&) noexcept
		//{}
		//virtual void Accept(TechniqueProbe&)
		//{}
		[[nodiscard]] virtual std::string GetUID() const noexcept
		{
			assert(false);
			return "";
		}
		virtual ~Bindable() = default;
	};

	class CloningBindable : public Bindable
	{
	public:
		virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	};
}
