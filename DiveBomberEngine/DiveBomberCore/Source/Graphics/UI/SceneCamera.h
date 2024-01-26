#pragma once
#include "UIWidget.h"

namespace DiveBomber::UI
{
	class SceneCamera final : public UIWidget
	{
	public:
		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() const noexcept override
		{
			return "Scene Camera";
		}
		[[nodiscard]] bool GetIsUniqueUI() const noexcept override
		{
			return true;
		}
	private:
		int currentSelectedIndex = -1;
	};
}