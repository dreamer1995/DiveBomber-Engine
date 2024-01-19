#include "MainMenuBar.h"

#include "ObjectDetail.h"
#include "ResourceBrowser.h"
#include "SceneOutliner.h"
#include "UIManager.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::UI
{
	void MainMenuBar::DrawUI()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Create"))
				{
				}
				if (ImGui::MenuItem("Open", "Ctrl+O"))
				{
				}
				if (ImGui::MenuItem("Save", "Ctrl+S"))
				{
				}
				if (ImGui::MenuItem("Save as.."))
				{
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Object Detail"))
				{
					UIManager::GetInstance().AddToUIDrawList(std::make_shared<ObjectDetail>());
				}
				if (ImGui::MenuItem("Resource Browser"))
				{
					UIManager::GetInstance().AddToUIDrawList(std::make_shared<ResourceBrowser>());
				}
				if (ImGui::MenuItem("Scene Outliner"))
				{
					UIManager::GetInstance().AddToUIDrawList(std::make_shared<SceneOutliner>());
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}