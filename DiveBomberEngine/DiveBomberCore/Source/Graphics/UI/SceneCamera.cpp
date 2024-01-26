#include "SceneCamera.h"

#include "..\..\DiveBomberCore.h"
#include "..\..\Graphics\Graphics.h"
#include "..\..\Component\Camera\Camera.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::UI
{
	using namespace DEGraphics;

	void SceneCamera::DrawUI()
	{
		if(isShown)
		{
			std::string captionChar = GetCaption() + (id == 1 ? "" : " " + std::to_string(id));
			ImGui::Begin(captionChar.c_str(), &isShown);
				Graphics::GetInstance().GetCamera()->DrawComponentUI();
			ImGui::End();
		}
	}
}