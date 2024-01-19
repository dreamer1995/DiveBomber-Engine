#include "ResourceBrowser.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::UI
{
	void ResourceBrowser::DrawUI()
	{
		if (isShown)
		{
			std::string captionChar = GetCaption() + (id == 1 ? "" : " " + std::to_string(id));
			ImGui::Begin(captionChar.c_str(), &isShown);
			ImGui::End();
		}
	}
}