#include "ObjectDetail.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::UI
{
	void ObjectDetail::DrawUI()
	{
		if (isShown)
		{
			std::string captionChar = GetCaption() + " " + std::to_string(id);
			ImGui::Begin(captionChar.c_str(), &isShown);
			ImGui::Text("Object Name");
			ImGui::End();
		}
	}
}