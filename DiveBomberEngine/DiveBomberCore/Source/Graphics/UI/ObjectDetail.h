#pragma once
#include "UIWidget.h"

namespace DiveBomber::UI
{
	class ObjectDetail final : public UIWidget
	{
	public:
		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() const noexcept override
		{
			return "Object Detail";
		}
		[[nodiscard]] bool GetIsUniqueUI() const noexcept override
		{
			return false;
		}
	};
}