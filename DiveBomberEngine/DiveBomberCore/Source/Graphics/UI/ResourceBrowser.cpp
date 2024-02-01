#include "ResourceBrowser.h"

#include "..\..\Utility\GlobalParameters.h"
#include "..\..\Utility\Common.h"
#include "..\..\Window\Window.h"
#include "..\..\Hardware\Keyboard.h"
#include "..\Resource\ShaderInputable\Texture.h"
#include "..\DX\GlobalResourceManager.h"
#include "..\Graphics.h"

#include <..\imgui\imgui.h>
#include <iostream>

namespace DiveBomber::UI
{
	using namespace DEWindow;
	using namespace DEResource;
	using namespace DX;
	using namespace DEGraphics;

	ResourceBrowser::ResourceBrowser()
	{
		RecursiveFilePath(ProjectDirectoryW, fileTree);
		selectedTreeNodeStack.push(&fileTree);
		fileTree.expanded = true;

		iconAtlas = GlobalResourceManager::Resolve<Texture>(L"MyImage01.jpg");
	}

	ResourceBrowser::~ResourceBrowser()
	{
	}

	void ResourceBrowser::DrawUI()
	{
		if (isShown)
		{
			ImGui::Begin("DirectX12 Texture Test");
			ImGui::Text("CPU handle = %p", iconAtlas->GetSRVDescriptorCPUHandle());
			ImGui::Text("GPU handle = %p", iconAtlas->GetSRVDescriptorGPUHandle());
			auto texDesc = iconAtlas->GetTextureBuffer()->GetDesc();
			ImGui::Text("size = %d x %d", texDesc.Width, texDesc.Height);
			// Note that we pass the GPU SRV handle here, *not* the CPU handle. We're passing the internal pointer value, cast to an ImTextureID
			ImGui::Image((ImTextureID)iconAtlas->GetSRVDescriptorGPUHandle().ptr, ImVec2((float)texDesc.Width, (float)texDesc.Height));
			ImGui::End();

			std::string captionChar = GetCaption() + (id == 1 ? "" : " " + std::to_string(id));
			ImGui::ShowDemoWindow();
			ImGui::Begin(captionChar.c_str(), &isShown);

			ImGui::BeginChild("ContentInfo", ImVec2(ImGui::GetContentRegionAvail().x, 0),
				ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize);
				if (ImGui::Button("Prev"))
				{
					if (selectedTreeNodeStack.size() > 1)
					{
						selectedTreeNodeStack.pop();
					}
				}
				ImGui::SameLine();
				ImGui::Button("New");
				ImGui::SameLine(); 
				if (ImGui::Button(browserFileIconMode ? "Icon Mode" : "List Mode"))
				{
					browserFileIconMode = !browserFileIconMode;
				}
				ImGui::SameLine();
				std::string displayedPath = fs::absolute(selectedTreeNodeStack.top()->path).string();
				ImGui::Text(displayedPath.c_str());
			ImGui::EndChild();

			ImGui::BeginChild("ContentTree",
				ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y),
				ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX, ImGuiWindowFlags_MenuBar);
				if (ImGui::BeginMenuBar())
				{
					ImGui::TextUnformatted("ContentTree");
					ImGui::EndMenuBar();
				}

				if (ImGui::BeginListBox("##ContentTree", ImGui::GetContentRegionAvail()))
				{
					DrawContentTree(fileTree, 0u);
					ImGui::EndListBox();
				}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild("Contents",
				ImVec2(0, ImGui::GetContentRegionAvail().y),
				ImGuiChildFlags_Border, ImGuiWindowFlags_MenuBar);
				if (ImGui::BeginMenuBar())
				{
					ImGui::TextUnformatted("Contents");
					ImGui::EndMenuBar();
				}

				if (ImGui::BeginListBox("##Contents", ImGui::GetContentRegionAvail()))
				{
					
					DrawContents(*selectedTreeNodeStack.top());
					ImGui::EndListBox();
				}
			ImGui::EndChild();

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

			bool selected = inputTree.id == selectedTreeNodeStack.top()->id;
			std::string displayedName = inputTree.path.filename().empty() ? "Content" : inputTree.path.filename().string();
			
			const float indentSpace = 20.0f;
			ImGui::Indent(indentLevel * indentSpace);
			if (ImGui::Selectable(tag(displayedName), selected))
			{
				if (selectedTreeNodeStack.top() != &inputTree)
				{
					selectedTreeNodeStack.push(&inputTree);
				}
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
		ImVec2 selectBGSize = ImVec2(0, 0);

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

			if (browserFileIconMode)
			{
				ImGui::BeginChild(tag("FileOutFrame"), itemSize, ImGuiChildFlags_None);
					selectBGSize = ImGui::GetContentRegionAvail();
					ImGui::SetNextItemAllowOverlap();
					ImGui::BeginChild(tag("SelectableFrame"), ImGui::GetContentRegionAvail(), ImGuiChildFlags_None, ImGuiWindowFlags_NoMouseInputs);
						float windowWidth = ImGui::GetWindowSize().x;
						float windowHeight = ImGui::GetWindowSize().y;
						float textWidth = ImGui::CalcTextSize(child.path.stem().string().c_str()).x;
						float textHeight = ImGui::CalcTextSize(child.path.stem().string().c_str()).y;
						ImGui::SetCursorPosX((windowWidth - iconSize.x) * 0.5f);
						ImGui::SetCursorPosY((windowHeight - iconSize.y - textHeight) * 0.5f);
						ImGui::BeginChild(tag("IconFrame"), iconSize, ImGuiChildFlags_None, ImGuiWindowFlags_NoMouseInputs);
							ImDrawList* draw_list = ImGui::GetWindowDrawList();
							ImVec2 pos = ImGui::GetWindowPos();
							draw_list->AddRectFilledMultiColor(pos, ImVec2(pos.x + iconSize.x, pos.y + iconSize.y), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));
						ImGui::EndChild();
					
						if (textWidth < itemSize.x)
						{
							ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
						}
						ImGui::Text(child.path.stem().string().c_str());
					ImGui::EndChild();

					ImGui::SameLine();
			}
					bool selected = currentSelectedFileIDs.find(child.id) != currentSelectedFileIDs.end();
					if (ImGui::Selectable(browserFileIconMode? tag("##ItemSelectable") : tag(child.path.filename().string()),
						selected,
						ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick,
						selectBGSize))
					{
						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							if (child.children.size() > 0)
							{
								currentSelectedFileIDs.clear();
								if (selectedTreeNodeStack.top() != &child)
								{
									selectedTreeNodeStack.push(&child);
								}
								inputTree.expanded = true;
								child.expanded = true;
								if (browserFileIconMode)
								{
									ImGui::EndChild();
								}
								return;
							}
						}
						else
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
					}
			if (browserFileIconMode)
			{
				ImGui::EndChild();

				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + ImGui::GetStyle().ItemSpacing.x + itemSize.x;
				if ((i + 1) < (int)inputTree.children.size() && next_button_x2 < window_visible_x2)
				{
					ImGui::SameLine();
				}
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