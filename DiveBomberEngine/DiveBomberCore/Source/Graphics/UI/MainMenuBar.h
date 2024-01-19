#pragma once
#include "UIWidget.h"

namespace DiveBomber::UI
{
	class MainMenuBar final : public UIWidget
	{
	public:
		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() const noexcept override
		{
			return "Main MenuBar";
		}
		[[nodiscard]] bool GetIsUniqueUI() const noexcept override
		{
			return true;
		}
	};
}