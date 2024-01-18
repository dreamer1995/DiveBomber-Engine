#include "MainMenuBar.h"

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
			ImGui::EndMainMenuBar();
		}
	}
}