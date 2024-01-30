#include "ResourceBrowser.h"

#include "..\..\Utility\GlobalParameters.h"
#include "..\..\Utility\Common.h"
#include "..\..\Window\Window.h"
#include "..\..\Hardware\Keyboard.h"

#include <..\imgui\imgui.h>
#include <iostream>

namespace DiveBomber::UI
{
	using namespace DEWindow;

	ResourceBrowser::ResourceBrowser()
	{
		RecursiveFilePath(ProjectDirectoryW, fileTree);
		currentSelectedTreeNode = fileTree;
		fileTree.expanded = true;
	}

	void ResourceBrowser::DrawUI()
	{
		if (isShown)
		{
			std::string captionChar = GetCaption() + (id == 1 ? "" : " " + std::to_string(id));
			ImGui::ShowDemoWindow();
			ImGui::Begin(captionChar.c_str(), &isShown);

			if (ImGui::BeginChild("ContentInfo", ImVec2(ImGui::GetContentRegionAvail().x, 0),
				ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize))
			{
				ImGui::Button("Prev");
				ImGui::SameLine();
				ImGui::Button("New");
				ImGui::SameLine();
				std::string displayedPath = fs::absolute(currentSelectedTreeNode.path).string();
				ImGui::Text(displayedPath.c_str());
				ImGui::EndChild();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			if (ImGui::BeginChild("ContentTree",
				ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y),
				ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX))
			{
				ImVec2 listBoxSize = ImGui::GetContentRegionAvail();

				if (ImGui::BeginListBox("##ContentTree", listBoxSize))
				{
					DrawContentTree(fileTree, 0u);
					ImGui::EndListBox();
				}
				ImGui::EndChild();
			}
			ImGui::PopStyleVar();

			ImGui::SameLine();

			if (ImGui::BeginChild("Contents",
				ImVec2(0, ImGui::GetContentRegionAvail().y),
				ImGuiChildFlags_Border))
			{
				ImVec2 listBoxSize = ImGui::GetContentRegionAvail();

				if (ImGui::BeginListBox("##Contents", listBoxSize))
				{
					DrawContents(currentSelectedTreeNode);
					ImGui::EndListBox();
				}
				ImGui::EndChild();
			}

			ImGui::End();
		}
	}

	void ResourceBrowser::DrawContentTree(FileTreeNode& inputTree, UINT indentLevel)
	{
		if (inputTree.children.size() > 0)
		{
			using namespace std::string_literals;
			auto tag = [tagScratch = std::string{}, tagString = "##"s +std::to_string(inputTree.id)]
			(std::string label) mutable
			{
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			bool selected = inputTree.id == currentSelectedTreeNode.id;
			std::string displayedName = inputTree.path.filename().empty() ? "Content" : inputTree.path.filename().string();
			
			const float indentSpace = 20.0f;
			ImGui::Indent(indentLevel * indentSpace);
			if (ImGui::Selectable(tag(displayedName), selected))
			{
				currentSelectedTreeNode = inputTree;
				if (selected)
				{
					inputTree.expanded = !inputTree.expanded;
				}
				currentSelectedFileIDs.clear();
			}
			ImGui::Unindent(indentLevel * indentSpace);

			if (inputTree.expanded)
			{
				for (auto& child : inputTree.children)
				{
					DrawContentTree(child, indentLevel + 1u);
				}
			}
		}
	}

	void ResourceBrowser::DrawContents(FileTreeNode& inputTree)
	{
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		ImVec2 itemSize = ImVec2(80, 80);
		ImVec2 iconSize = ImVec2(45, 45);

		bool checkSelect = false;
		for (int i = 0; i < inputTree.children.size(); i++)
		{
			FileTreeNode& child = inputTree.children[i];

			using namespace std::string_literals;
			auto tag = [tagScratch = std::string{}, tagString = "##"s + std::to_string(child.id)]
			(std::string label) mutable
			{
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			if (ImGui::BeginChild(tag("FileOutFrame"), itemSize, ImGuiChildFlags_None))
			{
				ImVec2 BGSize = ImGui::GetContentRegionAvail();
				ImGui::SetNextItemAllowOverlap();
				if(ImGui::BeginChild(tag("SelectableFrame"), ImGui::GetContentRegionAvail(), ImGuiChildFlags_Border, ImGuiWindowFlags_NoMouseInputs))
				{
					ImGui::BeginChild(tag("IconFrame"), iconSize, ImGuiChildFlags_None, ImGuiWindowFlags_NoMouseInputs);
						ImDrawList* draw_list = ImGui::GetWindowDrawList();
						ImVec2 pos = ImGui::GetWindowPos();
						draw_list->AddRectFilledMultiColor(pos, ImVec2(pos.x + iconSize.x, pos.y + iconSize.y), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));
					ImGui::EndChild();
					
					ImGui::Text(child.path.filename().string().c_str());
					ImGui::EndChild();
				}
				ImGui::SameLine();
				bool selected = currentSelectedFileIDs.find(child.id) != currentSelectedFileIDs.end();
				if (ImGui::Selectable(tag("##ItemSelectable"), selected,
					ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap,
					BGSize))
				{
					if (selected)
					{
						// aware !
						if (!Window::GetInstance().kbd->KeyIsDown(VK_CONTROL))
						{
							currentSelectedFileIDs.clear();
							currentSelectedFileIDs.emplace(child.id);
							checkSelect = true;
						}
						else
						{
							currentSelectedFileIDs.erase(child.id);
						}
					}
					else
					{
						if (!Window::GetInstance().kbd->KeyIsDown(VK_CONTROL))
						{
							currentSelectedFileIDs.clear();
						}
						currentSelectedFileIDs.emplace(child.id);
						checkSelect = true;
					}
				}
				ImGui::EndChild();
			}

			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + ImGui::GetStyle().ItemSpacing.x + itemSize.x;
			if ((i + 1) < inputTree.children.size() && next_button_x2 < window_visible_x2)
			{
				ImGui::SameLine();
			}
		}

		// When mouse within window, mouse released on nothing.
		if (ImGui::IsMouseReleased(0) && ImGui::IsWindowHovered() && !checkSelect)
		{
			currentSelectedFileIDs.clear();
		}
	}

	void ResourceBrowser::RecursiveFilePath(fs::path path, FileTreeNode& inputFileTree) noexcept
	{
		inputFileTree.id = fileTreeIDCounter++;
		inputFileTree.path = path;
		for (auto const& dir_entry : fs::directory_iterator(path))
		{
			FileTreeNode childFileNode;

			if (dir_entry.is_directory())
			{
				//if (dir_entry.path().filename() == L"Cache")
				//{
				//	continue;
				//}
				RecursiveFilePath(dir_entry, childFileNode);
			}
			else
			{
				childFileNode.id = fileTreeIDCounter++;
				childFileNode.path = dir_entry;
			}

			inputFileTree.children.emplace_back(childFileNode);
		}
	}
}