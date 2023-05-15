#pragma once
#include "..\Graphics.h"

#include <memory>
#include <string>
#include <filesystem>

namespace DiveBomber::BindObj
{
	class Bindable
	{
	public:
		virtual void Bind(DEGraphics::Graphics& gfx) noxnd = 0;
		//virtual void InitializeParentReference(const Drawable&) noexcept
		//{}
		//virtual void Accept(TechniqueProbe&)
		//{}
		virtual std::string GetUID() const noexcept
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
