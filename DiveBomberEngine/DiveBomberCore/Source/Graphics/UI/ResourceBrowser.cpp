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

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			if (ImGui::BeginChild("ContentTree",
				ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y),
				ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX))
			{
				ImVec2 listBoxSize = ImGui::GetContentRegionAvail();

				if (ImGui::BeginListBox("##", listBoxSize))
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

				if (ImGui::BeginListBox("##", listBoxSize))
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
				inputTree.expanded = !inputTree.expanded;
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
		for (auto& child : inputTree.children)
		{
			using namespace std::string_literals;
			auto tag = [tagScratch = std::string{}, tagString = "##"s + std::to_string(child.id)]
			(std::string label) mutable
			{
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			bool selected = currentSelectedFileIDs.find(child.id) != currentSelectedFileIDs.end();
			if (ImGui::Selectable(tag(child.path.filename().string()), selected))
			{
				if (selected)
				{
					currentSelectedFileIDs.erase(child.id);
				}
				else
				{
					#define	wnd Window::GetInstance()
					if (!wnd.kbd->KeyIsDown(VK_CONTROL))
					{
						currentSelectedFileIDs.clear();
					}
					currentSelectedFileIDs.emplace(child.id);
				}
			}
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