#include "UIManager.h"

#include "MainMenuBar.h"
#include "ObjectDetail.h"
#include "ResourceBrowser.h"
#include "SceneOutliner.h"
#include "RenderStatusPanel.h"

namespace DiveBomber::UI
{
	std::unique_ptr<UIManager> UIManager::instance;

	UIManager::UIManager()
	{
		mainMenuBar = std::make_shared<MainMenuBar>();

		AddToUIDrawList(std::make_shared<ObjectDetail>());
		AddToUIDrawList(std::make_shared<ResourceBrowser>());
		AddToUIDrawList(std::make_shared<SceneOutliner>());
		AddToUIDrawList(std::make_shared<RenderStatusPanel>());
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
		mainMenuBar->DrawUI();
		for (auto& UI : UIDrawList)
		{
			UI.second->DrawUI();
		}
	}

	void UIManager::AddToUIDrawList(const std::shared_ptr<UIWidget> widget)
	{
		using namespace std::string_literals;

		if (UIDrawList.size() == 0 || widget->GetIsUniqueUI())
		{
			const std::string questKey = widget->GetCaption() + " " + std::to_string(1);
			UIDrawList[questKey] = widget;
			widget->SetID(1);
			return;
		}

		for (int i = 1; i < UIDrawList.size() + 1; i++)
		{
			const std::string questKey = widget->GetCaption() + " " + std::to_string(i);
			auto iterator = UIDrawList.find(questKey);
			if (iterator == UIDrawList.end())
			{
				UIDrawList[questKey] = widget;
				widget->SetID(i);
				break;
			}
			else
			{
				if (iterator->second->IsShown() != true)
				{
					UIDrawList[questKey] = widget;
					widget->SetID(i);
					break;
				}
			}
		}
	}
}