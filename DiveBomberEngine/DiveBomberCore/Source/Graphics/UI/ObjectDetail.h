#pragma once
#include "UIWidget.h"

namespace DiveBomber::UI
{
	class ObjectDetail final : public UIWidget
	{
	public:
		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() override
		{
			return "Object Detail";
		}
		[[nodiscard]] bool GetIsUniqueUI() override
		{
			return false;
		}
	};
}