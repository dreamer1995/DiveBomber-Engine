#include "UIManager.h"

#include "MainMenuBar.h"
#include "ObjectDetail.h"
#include "ResourceBrowser.h"
#include "SceneOutliner.h"

namespace DiveBomber::UI
{
	std::unique_ptr<UIManager> UIManager::instance;

	UIManager::UIManager()
	{
		mainMenuBar = std::make_shared<MainMenuBar>();
		objectDetail = std::make_shared<ObjectDetail>();
		resourceBrowser = std::make_shared<ResourceBrowser>();
		sceneOutliner = std::make_shared<SceneOutliner>();

		UIDrawList.emplace_back(mainMenuBar);
		UIDrawList.emplace_back(objectDetail);
		UIDrawList.emplace_back(resourceBrowser);
		UIDrawList.emplace_back(sceneOutliner);
	}

	UIManager& UIManager::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = std::make_unique<UIManager>();
		}
		return *instance;
	}

	void UIManager::Destructor() noexcept
	{
		if (instance != nullptr)
		{
			instance.reset();
		}
	}

	void UIManager::DrawUI()
	{
		for (std::shared_ptr<UIWidget>& UI : UIDrawList)
		{
			UI->DrawUI();
		}
	}
}