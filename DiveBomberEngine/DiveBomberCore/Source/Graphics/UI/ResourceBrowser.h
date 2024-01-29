#pragma once
#include "UIWidget.h"

#include <filesystem>
#include <unordered_set>

namespace DiveBomber::UI
{
	namespace fs = std::filesystem;

	class ResourceBrowser final : public UIWidget
	{
	public:
		ResourceBrowser();

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
		FileTreeNode currentSelectedTreeNode;
		std::unordered_set<UINT> currentSelectedFileIDs;
	};
}