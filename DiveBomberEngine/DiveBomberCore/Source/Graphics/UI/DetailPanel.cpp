#include "DetailPanel.h"

#include "..\..\DiveBomberCore.h"
#include "..\UI\DetailModifier.h"
#include "..\..\Resource.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::UI
{
	void DetailPanel::DrawUI()
	{
		if (isShown)
		{
			std::string captionChar = GetCaption() + (id == 1 ? "" : " " + std::to_string(id));
			ImGui::Begin(captionChar.c_str(), &isShown);
			std::shared_ptr<UI::DetailModifier> detailModifier = DiveBomberCore::GetInstance().GetCurrentSelectedDetail();
			if (detailModifier)
			{
				std::shared_ptr<Resource> resource = std::dynamic_pointer_cast<Resource>(detailModifier);
				if (resource)
				{
					ImGui::Text(Utility::ToNarrow(resource->GetName()).c_str());
				}

				detailModifier->DrawDetailPanel();
			}

			ImGui::End();
		}
	}
}