#include "SceneOutliner.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::UI
{
	void SceneOutliner::DrawUI()
	{
		if (isShown)
		{
			std::string captionChar = GetCaption() + " " + std::to_string(id);
			ImGui::Begin(captionChar.c_str(), &isShown);
			ImGui::End();
		}
	}
}