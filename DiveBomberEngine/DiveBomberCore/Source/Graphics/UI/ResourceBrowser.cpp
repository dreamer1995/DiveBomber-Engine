#include "ResourceBrowser.h"

#include "..\..\Utility\GlobalParameters.h"
#include "..\..\Utility\Common.h"
#include "..\..\Window\Window.h"
#include "..\..\Hardware\Keyboard.h"
#include "..\Resource\ShaderInputable\Texture.h"
#include "..\DX\GlobalResourceManager.h"
#include "..\Graphics.h"

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

		iconAtlas = GlobalResourceManager::Resolve<Texture>(L"UIIcons.png");
		Graphics::GetInstance().ExecuteAllCurrentCommandLists();
	}

	ResourceBrowser::~ResourceBrowser()
	{
	}

	void ResourceBrowser::DrawUI()
	{
		if (isShown)
		{
			std::string captionChar = GetCaption() + (id == 1 ? "" : " " + std::to_string(id));
			ImGui::ShowDemoWindow();
			ImGui::Begin(captionChar.c_str(), &isShown);

			ImGui::BeginChild("ContentInfo", ImVec2(ImGui::GetContentRegionAvail().x, 0),
				ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize);
				ImVec2 buttonSize = ImVec2(20, 20);
				{
					ImVec2 uv0;
					ImVec2 uv1;
					GetSpecificIconUVFromAtlas(0u, (UINT)buttonSize.x, uv0, uv1);
					if (ImGui::ImageButton("##Prev", (ImTextureID)iconAtlas->GetSRVDescriptorGPUHandle().ptr, buttonSize, uv0, uv1))
					{
						if (selectedTreeNodeStack.size() > 1)
						{
							selectedTreeNodeStack.pop();
						}
					}
				}
				ImGui::SameLine();
				{
					ImVec2 uv0;
					ImVec2 uv1;
					GetSpecificIconUVFromAtlas(3u, (UINT)buttonSize.x, uv0, uv1);
					if (ImGui::ImageButton("##New", (ImTextureID)iconAtlas->GetSRVDescriptorGPUHandle().ptr, buttonSize, uv0, uv1))
					{
					}
				}
				ImGui::SameLine(); 
				{
					UINT iconIndex = browserFileIconMode ? 5u : 4u;
					ImVec2 uv0;
					ImVec2 uv1;
					GetSpecificIconUVFromAtlas(iconIndex, (UINT)buttonSize.x, uv0, uv1);
					if (ImGui::ImageButton("##DisplayMode", (ImTextureID)iconAtlas->GetSRVDescriptorGPUHandle().ptr, buttonSize, uv0, uv1))
					{
						browserFileIconMode = !browserFileIconMode;
					}
				}
				ImGui::SameLine();
				std::string displayedPath = fs::absolute(selectedTreeNodeStack.top()->path).string();
				ImGui::SetCursorPosY((ImGui::GetWindowSize().y - ImGui::GetTextLineHeight()) * 0.5f);
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

			const ImVec2 listSize = ImVec2(0, 20);
			ImGui::SetNextItemAllowOverlap();
			ImGui::BeginChild(tag("TreeListOutFrame"), listSize, ImGuiChildFlags_ResizeX | ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoMouseInputs);
				const ImVec2 iconSize = ImVec2(listSize.y, listSize.y);

				UINT imageIndex = inputTree.expanded ? 2u : 1u;
				ImVec2 uv0;
				ImVec2 uv1;
				GetSpecificIconUVFromAtlas(imageIndex, (UINT)iconSize.x, uv0, uv1);

				ImGui::Image((ImTextureID)iconAtlas->GetSRVDescriptorGPUHandle().ptr,
					ImVec2(iconSize.x, iconSize.y), uv0, uv1);

				ImGui::SameLine();

				ImGui::SetCursorPosY((ImGui::GetWindowSize().y - ImGui::GetTextLineHeight()) * 0.5f);
				ImGui::Text(displayedName.c_str());
			ImGui::EndChild();

			ImGui::SameLine();

			if (ImGui::Selectable(tag("##TreeList"), selected,
				ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick,
				listSize))
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
		const ImVec2 itemSize = ImVec2(80, 80);
		const ImVec2 listSize = ImVec2(0, 20);
		const ImVec2 iconBlockSize = ImVec2(45, 45);
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
			
			UINT iconIndex = 0u;
			std::shared_ptr<DEResource::Texture> fileIconTexture;
			bool isFolderNode = false;
			if (child.children.size() > 0)
			{
				fileIconTexture = iconAtlas;
				iconIndex = 1u;
				isFolderNode = true;
			}
			else if (!child.path.stem().wstring().contains(L"#") && (
				child.path.extension() == L".dds" || 
				child.path.extension() == L".png" || 
				child.path.extension() == L".jpg" || 
				child.path.extension() == L".tga" || 
				child.path.extension() == L".hdr" || 
				child.path.extension() == L".exr"))
			{
				fileIconTexture = GlobalResourceManager::Resolve<Texture>
					(child.path.stem().wstring() + L"#DERBIcon" + child.path.extension().wstring());
				Graphics::GetInstance().ExecuteCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
			}
			else if (child.path.stem().wstring().contains(L"#DERBIcon"))
			{
				fileIconTexture = GlobalResourceManager::Resolve<Texture>(child.path.filename());
				Graphics::GetInstance().ExecuteCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
			}
			else
			{
				fileIconTexture = iconAtlas;
				iconIndex = 2u;
				isFolderNode = true;
			}

			const D3D12_RESOURCE_DESC texDesc = fileIconTexture->GetTextureBuffer()->GetDesc();
			const float XYRatio = texDesc.Width / (float)texDesc.Height;
			const float textWidth = ImGui::CalcTextSize(child.path.stem().string().c_str()).x;

			if (browserFileIconMode)
			{
				ImGui::BeginChild(tag("FileOutFrame"), itemSize, ImGuiChildFlags_None);
					selectBGSize = ImGui::GetContentRegionAvail();
					ImGui::SetNextItemAllowOverlap();
					ImGui::BeginChild(tag("SelectableFrame"), ImGui::GetContentRegionAvail(), ImGuiChildFlags_None, ImGuiWindowFlags_NoMouseInputs);
						float windowWidth = ImGui::GetWindowSize().x;
						float windowHeight = ImGui::GetWindowSize().y;
						ImGui::SetCursorPosX((windowWidth - iconBlockSize.x) * 0.5f);
						ImGui::SetCursorPosY((windowHeight - iconBlockSize.y - ImGui::GetTextLineHeight()) * 0.5f);
						ImGui::BeginChild(tag("IconFrame"), iconBlockSize, ImGuiChildFlags_None, ImGuiWindowFlags_NoMouseInputs);
							ImVec2 iconSize = iconBlockSize;
							ImVec2 uv0 = ImVec2(0, 0);
							ImVec2 uv1 = ImVec2(1, 1);
							if (!isFolderNode)
							{
								if (XYRatio > 1)
								{
									iconSize.y /= XYRatio;
								}
								else
								{
									iconSize.x *= XYRatio;
								}
							}
							else
							{
								GetSpecificIconUVFromAtlas(iconIndex, (UINT)iconSize.x, uv0, uv1);
							}

							ImGui::SetCursorPosX((ImGui::GetWindowSize().x - iconSize.x) * 0.5f);
							ImGui::SetCursorPosY((ImGui::GetWindowSize().y - iconSize.y) * 0.5f);
							ImGui::Image((ImTextureID)fileIconTexture->GetSRVDescriptorGPUHandle().ptr,
								ImVec2(iconSize.x, iconSize.y), uv0, uv1);
						ImGui::EndChild();
					
						if (textWidth < itemSize.x)
						{
							ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
						}
						ImGui::Text(child.path.stem().string().c_str());
					ImGui::EndChild();

					ImGui::SameLine();
			}
			else
			{
					selectBGSize = listSize;
					ImGui::SetNextItemAllowOverlap();
					ImGui::BeginChild(tag("FileOutFrame"), listSize, ImGuiChildFlags_ResizeX | ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoMouseInputs);
						ImVec2 iconSize = ImVec2(listSize.y, listSize.y);
						ImVec2 uv0 = ImVec2(0, 0);
						ImVec2 uv1 = ImVec2(1, 1);
						if (!isFolderNode)
						{
							if (XYRatio > 1)
							{
								iconSize.y /= XYRatio;
							}
							else
							{
								iconSize.x *= XYRatio;
							}
						}
						else
						{
							GetSpecificIconUVFromAtlas(iconIndex, (UINT)iconSize.x, uv0, uv1);
						}

						ImGui::Image((ImTextureID)fileIconTexture->GetSRVDescriptorGPUHandle().ptr,
							ImVec2(iconSize.x, iconSize.y), uv0, uv1);

						ImGui::SameLine();

						ImGui::SetCursorPosY((ImGui::GetWindowSize().y - ImGui::GetTextLineHeight()) * 0.5f);
						ImGui::Text(child.path.filename().string().c_str());
					ImGui::EndChild();

					ImGui::SameLine();
			}

					bool selected = currentSelectedFileIDs.find(child.id) != currentSelectedFileIDs.end();
					if (ImGui::Selectable(tag("##ItemSelectable"),
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

	void ResourceBrowser::GetSpecificIconUVFromAtlas(UINT index, UINT iconSize, ImVec2& uv0, ImVec2& uv1) noexcept
	{

		const UINT xNumIcons = 2;
		const UINT yNumIcons = 4;
		const UINT baseMipSize = 128u;
		const ImVec2 baseMipUVSize = ImVec2((2.0f / 3.0f) / xNumIcons, 1.0f / yNumIcons);

		float xStart = (index / yNumIcons) / (float)xNumIcons;
		float yStart = (index % yNumIcons) / (float)yNumIcons;

		UINT mipLevel = 0;
		while (iconSize < (baseMipSize >> (mipLevel + 1)))
		{
			mipLevel++;
		}
		mipLevel = mipLevel > 2 ? 2 : mipLevel;

		if (mipLevel < 1)
		{
			uv0 = ImVec2(xStart, yStart);
			uv1 = ImVec2(uv0.x + baseMipUVSize.x, uv0.y + baseMipUVSize.y);
		}
		else
		{
			uv0 = ImVec2(xStart + baseMipUVSize.x, yStart + baseMipUVSize.y / 2 * (mipLevel - 1));
			uv1 = ImVec2(uv0.x + baseMipUVSize.x / 2 / mipLevel, uv0.y + baseMipUVSize.y / 2 / mipLevel);
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