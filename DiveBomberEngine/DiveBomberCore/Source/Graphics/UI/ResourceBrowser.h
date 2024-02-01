#pragma once
#include "UIWidget.h"

#include <filesystem>
#include <unordered_set>
#include <stack>

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
		struct FileTreeNode
		{
			UINT id = MAXUINT32;
			fs::path path;
			std::vector<FileTreeNode> children;
			bool expanded = false;
		};

	private:
		void RecursiveFilePath(fs::path path, FileTreeNode& inputFileTree) noexcept;
		void DrawContentTree(FileTreeNode& inputTree, UINT indentLevel);
		void DrawContents(FileTreeNode& inputTree);

	private:
		UINT fileTreeIDCounter = 0;
		FileTreeNode fileTree;

		std::stack<FileTreeNode*> selectedTreeNodeStack;
		std::unordered_set<UINT> currentSelectedFileIDs;
		bool browserFileIconMode = true;
		std::shared_ptr<DEResource::Texture> iconAtlas;
	};
}