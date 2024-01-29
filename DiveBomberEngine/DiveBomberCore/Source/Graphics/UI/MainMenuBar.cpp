#include "MainMenuBar.h"

#include "UIManager.h"
#include "ObjectDetail.h"
#include "ResourceBrowser.h"
#include "SceneOutliner.h"
#include "RenderStatusPanel.h"
#include "SceneCamera.h"

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
				if (ImGui::MenuItem("Render Status"))
				{
					UIManager::GetInstance().AddToUIDrawList(std::make_shared<RenderStatusPanel>());
				}
				if (ImGui::MenuItem("Scene Camera"))
				{
					UIManager::GetInstance().AddToUIDrawList(std::make_shared<SceneCamera>());
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}