#include "ResourceBrowser.h"

#include "..\..\Utility\GlobalParameters.h"
#include "..\..\Utility\Common.h"
#include "..\..\Utility\DEJson.h"
#include "..\..\Window\Window.h"
#include "..\..\Hardware\Keyboard.h"
#include "..\Resource\ShaderInputable\Texture.h"
#include "..\DX\GlobalResourceManager.h"
#include "..\Graphics.h"

#include <iostream>
#include <fstream>

namespace DiveBomber::UI
{
	using namespace DEWindow;
	using namespace DEResource;
	using namespace DX;
	using namespace DEGraphics;

	using json = nlohmann::json;

	ResourceBrowser::ResourceBrowser()
	{
		iconAtlasTexture = GlobalResourceManager::Resolve<Texture>(L"UIIcons.png");
		Graphics::GetInstance().ExecuteAllCurrentCommandLists();

		backArrow = std::make_shared<Icon>();
		addNew = std::make_shared<Icon>();
		listMode = std::make_shared<Icon>();
		iconMode = std::make_shared<Icon>();
		closedFolder = std::make_shared<Icon>();
		openedFolder = std::make_shared<Icon>();
		fileIconClosedFolder = std::make_shared<Icon>();
		fileIconMaterial = std::make_shared<Icon>();

		SetIcon(backArrow, 0u, (UINT)buttonSize.x);
		SetIcon(addNew, 3u, (UINT)buttonSize.x);
		SetIcon(listMode, 4u, (UINT)buttonSize.x);
		SetIcon(iconMode, 5u, (UINT)buttonSize.x);

		SetIcon(closedFolder, 1u, (UINT)listSize.y);
		SetIcon(openedFolder, 2u, (UINT)listSize.y);

		RecursiveFilePath(ProjectDirectoryW, fileTree);
		selectedTreeNodeStack.push(&fileTree);
		fileTree.expanded = true;
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
				{
					if (ImGui::ImageButton("##Prev", (ImTextureID)backArrow->iconTexture->GetSRVDescriptorGPUHandle().ptr, buttonSize, backArrow->uv0, backArrow->uv1))
					{
						if (selectedTreeNodeStack.size() > 1)
						{
							selectedTreeNodeStack.pop();
						}
					}
				}
				ImGui::SameLine();
				{
					if (ImGui::ImageButton("##New", (ImTextureID)addNew->iconTexture->GetSRVDescriptorGPUHandle().ptr, buttonSize, addNew->uv0, addNew->uv1))
					{
					}
				}
				ImGui::SameLine(); 
				{
					std::shared_ptr<Icon> icon = browserFileIconMode ? iconMode : listMode;
					if (ImGui::ImageButton("##DisplayMode", (ImTextureID)icon->iconTexture->GetSRVDescriptorGPUHandle().ptr, buttonSize, icon->uv0, icon->uv1))
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
		if (fs::is_directory(inputTree.path))
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

			ImGui::SetNextItemAllowOverlap();
			ImGui::BeginChild(tag("TreeListOutFrame"), listSize, ImGuiChildFlags_ResizeX | ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoMouseInputs);
				const ImVec2 iconSize = ImVec2(listSize.y, listSize.y);

				std::shared_ptr<Icon> icon = inputTree.expanded ? openedFolder : closedFolder;

				ImGui::Image((ImTextureID)icon->iconTexture->GetSRVDescriptorGPUHandle().ptr,
					ImVec2(listSize.y, listSize.y), icon->uv0, icon->uv1);

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
		ImVec2 selectBGSize = ImVec2(0, 0);

		UINT iconFrameSize = browserFileIconMode ? (UINT)iconModeIconSize.x : (UINT)listSize.y;
		SetIcon(fileIconClosedFolder, 1u, iconFrameSize);
		SetIcon(fileIconMaterial, 7u, iconFrameSize);

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

			const float textWidth = ImGui::CalcTextSize(child.path.stem().string().c_str()).x;

			ImVec2 iconSize = child.icon->XYRatio;

			if (browserFileIconMode)
			{
				ImGui::BeginChild(tag("FileOutFrame"), itemSize, ImGuiChildFlags_None);
					selectBGSize = ImGui::GetContentRegionAvail();
					ImGui::SetNextItemAllowOverlap();
					ImGui::BeginChild(tag("SelectableFrame"), ImGui::GetContentRegionAvail(), ImGuiChildFlags_None, ImGuiWindowFlags_NoMouseInputs);
						float windowWidth = ImGui::GetWindowSize().x;
						float windowHeight = ImGui::GetWindowSize().y;
						ImGui::SetCursorPosX((windowWidth - iconModeIconSize.x) * 0.5f);
						ImGui::SetCursorPosY((windowHeight - iconModeIconSize.y - ImGui::GetTextLineHeight()) * 0.5f);
						ImGui::BeginChild(tag("IconFrame"), iconModeIconSize, ImGuiChildFlags_None, ImGuiWindowFlags_NoMouseInputs);
							iconSize = ImVec2(iconSize.x * iconModeIconSize.x, iconSize.y * iconModeIconSize.y);

							ImGui::SetCursorPosX((ImGui::GetWindowSize().x - iconSize.x) * 0.5f);
							ImGui::SetCursorPosY((ImGui::GetWindowSize().y - iconSize.y) * 0.5f);
							ImGui::Image((ImTextureID)child.icon->iconTexture->GetSRVDescriptorGPUHandle().ptr,
								ImVec2(iconSize.x, iconSize.y), child.icon->uv0, child.icon->uv1);
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
						iconSize = ImVec2(iconSize.x * listSize.y, iconSize.y * listSize.y);

						ImGui::SetCursorPosY((ImGui::GetWindowSize().y - iconSize.y) * 0.5f);
						ImGui::Image((ImTextureID)child.icon->iconTexture->GetSRVDescriptorGPUHandle().ptr,
							ImVec2(iconSize.x, iconSize.y), child.icon->uv0, child.icon->uv1);

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
							if (fs::is_directory(child.path))
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
					if (ImGui::BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonRight)) // <-- use last item id as popup id
					{
						currentSelectedFileIDs.clear();
						currentSelectedFileIDs.emplace(child.id);
						switch (child.fileNodeType)
						{
						case (UINT)ConfigFileType::CFT_Material:
							MaterialResourcePopup();
							break;
						case (UINT)ConfigFileType::CFT_Texture:
							TextureResourcePopup();
							break;
						}
						ImGui::SeparatorText("Standard");
						if (ImGui::MenuItem("Delete", NULL))
						{
							std::cout << "Delete " + child.path.stem().string() << std::endl;
						}
						ImGui::EndPopup();
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
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !checkSelect)
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

	void ResourceBrowser::SetIcon(std::shared_ptr<Icon> icon, UINT iconIndex, UINT size) noexcept
	{
		icon->iconTexture = iconAtlasTexture;
		GetSpecificIconUVFromAtlas(iconIndex, size, icon->uv0, icon->uv1);
	}

	void ResourceBrowser::MaterialResourcePopup()
	{
		ImGui::SeparatorText("Material");
		if (ImGui::MenuItem("Refresh", NULL))
		{
			std::cout << "Refresh Material!" << std::endl;
		}
	}

	void ResourceBrowser::TextureResourcePopup()
	{
		ImGui::SeparatorText("Texture");
		if (ImGui::MenuItem("Refresh", NULL))
		{
			std::cout << "Refresh Texture!" << std::endl;
		}
	}

	void ResourceBrowser::RecursiveFilePath(fs::path path, FileTreeNode& inputFileTree)
	{
		inputFileTree.id = fileTreeIDCounter++;
		inputFileTree.path = path;
		inputFileTree.icon = fileIconClosedFolder;
		for (auto const& dir_entry : fs::directory_iterator(path))
		{
			FileTreeNode childFileNode;

			if (dir_entry.is_directory())
			{
				if (dir_entry.path().filename() == L"Cache")
				{
					continue;
				}
				RecursiveFilePath(dir_entry, childFileNode);
			}
			else
			{
				childFileNode.id = fileTreeIDCounter++;
				childFileNode.path = dir_entry;

				if (childFileNode.path.extension() == L".deasset")
				{
					json config;
					std::ifstream rawFile(childFileNode.path);
					if (!rawFile.is_open())
					{
						throw std::exception("Unable to open config file");
					}
					rawFile >> config;
					rawFile.close();
					switch ((ConfigFileType)config["ConfigFileType"])
					{
					case ConfigFileType::CFT_Material:
						childFileNode.icon = fileIconMaterial;
						childFileNode.fileNodeType = (UINT)ConfigFileType::CFT_Material;
						break;
					case ConfigFileType::CFT_Texture:
						childFileNode.icon = std::make_shared<Icon>();
						childFileNode.icon->iconTexture = GlobalResourceManager::Resolve<Texture>
							(childFileNode.path.stem().wstring() + L"#DERBIcon" + childFileNode.path.extension().wstring());
						Graphics::GetInstance().ExecuteCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
						const D3D12_RESOURCE_DESC texDesc = childFileNode.icon->iconTexture->GetTextureBuffer()->GetDesc();
						const float XYRatio = texDesc.Width / (float)texDesc.Height;
						if (XYRatio > 1)
						{
							childFileNode.icon->XYRatio.y /= XYRatio;
						}
						else
						{
							childFileNode.icon->XYRatio.x *= XYRatio;
						}
						childFileNode.fileNodeType = (UINT)ConfigFileType::CFT_Texture;
						break;
					}
				}
				else
				{
					continue;
				}
			}

			inputFileTree.children.emplace_back(childFileNode);
		}
	}
}