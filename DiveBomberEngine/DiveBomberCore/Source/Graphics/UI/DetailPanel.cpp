#include "DetailPanel.h"

#include "..\..\DiveBomberCore.h"
#include "..\UI\DetailModifier.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::UI
{
	void DetailPanel::DrawUI()
	{
		if (isShown)
		{
			std::string captionChar = GetCaption() + (id == 1 ? "" : " " + std::to_string(id));
			ImGui::Begin(captionChar.c_str(), &isShown);
			std::shared_ptr<UI::DetailModifier> object = DiveBomberCore::GetInstance().GetCurrentSelectedDetail();
			if (object)
			{
				object->DrawDetailPanel();
			}

			ImGui::End();
		}
	}
}