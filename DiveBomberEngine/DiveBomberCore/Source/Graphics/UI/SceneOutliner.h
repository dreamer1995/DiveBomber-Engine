#pragma once
#include "UIWidget.h"

namespace DiveBomber::UI
{
	class SceneOutliner final : public UIWidget
	{
	public:
		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() const noexcept override
		{
			return "Scene Outliner";
		}
		[[nodiscard]] bool GetIsUniqueUI() const noexcept override
		{
			return false;
		}
	private:
		int currentSelectedIndex = -1;
	};
}