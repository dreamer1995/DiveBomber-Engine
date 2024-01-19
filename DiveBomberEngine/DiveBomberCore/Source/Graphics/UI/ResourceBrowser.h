#pragma once
#include "UIWidget.h"

namespace DiveBomber::UI
{
	class ResourceBrowser final : public UIWidget
	{
	public:
		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() const noexcept override
		{
			return "Resource Browser";
		}
		[[nodiscard]] bool GetIsUniqueUI() const noexcept override
		{
			return false;
		}
	};
}