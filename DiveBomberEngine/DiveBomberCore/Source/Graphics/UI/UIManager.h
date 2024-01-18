#pragma once

#include <memory>
#include <vector>

namespace DiveBomber::UI
{
	class UIWidget;
	class MainMenuBar;
	class ObjectDetail;
	class ResourceBrowser;
	class SceneOutliner;

	class UIManager final
	{
	public:
		UIManager();

		[[nodiscard]] static UIManager& GetInstance();
		void Destructor() noexcept;
		void DrawUI();

	private:
		static std::unique_ptr<UIManager> instance;
		std::vector<std::shared_ptr<UIWidget>> UIDrawList;
		std::shared_ptr<MainMenuBar> mainMenuBar;
		std::shared_ptr<ObjectDetail> objectDetail;
		std::shared_ptr<ResourceBrowser> resourceBrowser;
		std::shared_ptr<SceneOutliner> sceneOutliner;
	};
}