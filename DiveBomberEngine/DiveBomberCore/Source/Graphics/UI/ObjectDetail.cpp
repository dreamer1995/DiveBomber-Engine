#include "ObjectDetail.h"

#include "..\..\DiveBomberCore.h"
#include "..\Object\Object.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::UI
{
	void ObjectDetail::DrawUI()
	{
		if (isShown)
		{
			std::string captionChar = GetCaption() + (id == 1 ? "" : " " + std::to_string(id));
			ImGui::Begin(captionChar.c_str(), &isShown);
			std::shared_ptr<DEObject::Object> object = DiveBomberCore::GetInstance().GetCurrentSelectedObject();
			if (object)
			{
				ImGui::Text(Utility::ToNarrow(object->GetName()).c_str());

				object->DrawDetailPanel();
			}

			ImGui::End();
		}
	}
}