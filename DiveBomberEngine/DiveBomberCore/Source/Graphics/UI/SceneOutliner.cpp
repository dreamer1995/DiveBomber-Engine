#include "SceneOutliner.h"

#include "..\..\DiveBomberCore.h"
#include "..\..\Scene\Scene.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::UI
{
	void SceneOutliner::DrawUI()
	{
		if(isShown)
		{
			std::shared_ptr<DEScene::Scene> currentScene = DiveBomberCore::GetInstance().GetCurrentScene();
			auto sceneObjects = currentScene->GetSceneObjects();
			std::vector<std::wstring> sceneObjectNames;
			for (auto& object : sceneObjects)
			{
				sceneObjectNames.emplace_back(object.first);
			}

			std::string captionChar = GetCaption() + (id == 1 ? "" : " " + std::to_string(id));
			ImGui::Begin(captionChar.c_str(), &isShown);

			ImVec2 listBoxSize = ImGui::GetContentRegionAvail();

			ImGui::Text(Utility::ToNarrow(currentScene->GetName()).c_str());
			listBoxSize.y -= ImGui::GetTextLineHeightWithSpacing();
			if (ImGui::BeginListBox("##SceneOutliner", listBoxSize))
			{
				bool checkSelect = false;
				for (int i = 0; i < sceneObjectNames.size(); i++)
				{
					bool selected = i == currentSelectedIndex;
					if (ImGui::Selectable(Utility::ToNarrow(sceneObjectNames[i]).c_str(), selected))
					{
						currentSelectedIndex = i;
						checkSelect = true;
					}
				}
				// When mouse within window, mouse released on nothing.
				if (ImGui::IsMouseReleased(0) && ImGui::IsWindowHovered() && !checkSelect)
				{
					currentSelectedIndex = -1;
				}
				ImGui::EndListBox();
			}
			ImGui::End();

			if (currentSelectedIndex != -1)
			{
				DiveBomberCore::GetInstance().SetCurrentSelectedObject(std::next(sceneObjects.begin(), currentSelectedIndex)->second);
			}
			else
			{
				DiveBomberCore::GetInstance().SetCurrentSelectedObject(nullptr);
			}
		}
	}
}