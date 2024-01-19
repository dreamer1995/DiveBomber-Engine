#pragma once
#include "UIWidget.h"

namespace DiveBomber::UI
{
	class ResourceBrowser final : public UIWidget
	{
	public:
		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() override
		{
			return "Resource Browser";
		}
		[[nodiscard]] bool GetIsUniqueUI() override
		{
			return false;
		}
	};
}