#pragma once
#include "UIWidget.h"

#include <filesystem>
#include <unordered_set>
#include <stack>
#include <..\imgui\imgui.h>

namespace DiveBomber::DEResource
{
	class Texture;
}

namespace DiveBomber::UI
{
	namespace fs = std::filesystem;

	class ResourceBrowser final : public UIWidget
	{
	public:
		enum class ConfigFileType
		{
			CFT_Material,
			CFT_Texture,
		};
	public:
		ResourceBrowser();
		~ResourceBrowser();

		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() const noexcept override
		{
			return "Resource Browser";
		}
		[[nodiscard]] bool GetIsUniqueUI() const noexcept override
		{
			return false;
		}

	private:
		struct Icon
		{
			std::shared_ptr<DEResource::Texture> iconTexture;
			ImVec2 uv0 = ImVec2(0, 0);
			ImVec2 uv1 = ImVec2(1, 1);
		};

		struct FileTreeNode
		{
			UINT id = MAXUINT32;
			fs::path path;
			std::vector<FileTreeNode> children;
			bool expanded = false;
			Icon icon;
		};

	private:
		void RecursiveFilePath(fs::path path, FileTreeNode& inputFileTree) noexcept;
		void DrawContentTree(FileTreeNode& inputTree, UINT indentLevel);
		void DrawContents(FileTreeNode& inputTree);
		void GetSpecificIconUVFromAtlas(UINT index, UINT iconSize, ImVec2& uv0, ImVec2& uv1) noexcept;
		void SetIcon(Icon& icon, UINT iconIndex, UINT size) noexcept;

	private:
		UINT fileTreeIDCounter = 0;
		FileTreeNode fileTree;

		std::stack<FileTreeNode*> selectedTreeNodeStack;
		std::unordered_set<UINT> currentSelectedFileIDs;
		bool browserFileIconMode = true;
		std::shared_ptr<DEResource::Texture> iconAtlasTexture;

		const ImVec2 buttonSize = ImVec2(20, 20);
		Icon backArrow;
		Icon addNew;
		Icon listMode;
		Icon iconMode;

		const ImVec2 listSize = ImVec2(0, 20);
		Icon closedFolder;
		Icon openedFolder;

		const ImVec2 iconModeIconSize = ImVec2(45, 45);
		Icon iconModeClosedFolder;
	};
}