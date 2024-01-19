#pragma once
#include "UIWidget.h"

namespace DiveBomber::UI
{
	class SceneOutliner final : public UIWidget
	{
	public:
		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() override
		{
			return "Scene Outliner";
		}
		[[nodiscard]] bool GetIsUniqueUI() override
		{
			return false;
		}
	};
}