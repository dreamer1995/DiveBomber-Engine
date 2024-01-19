#pragma once

#include <memory>
#include <unordered_map>
#include <string>

namespace DiveBomber::UI
{
	class UIWidget;
	class MainMenuBar;

	class UIManager final
	{
	public:
		UIManager();

		[[nodiscard]] static UIManager& GetInstance();
		static void Destructor() noexcept;
		void DrawUI();
		void AddToUIDrawList(const std::shared_ptr<UIWidget> widget);

	private:
		static std::unique_ptr<UIManager> instance;
		std::unordered_map<std::string, std::shared_ptr<UIWidget>> UIDrawList;
		std::shared_ptr<MainMenuBar> mainMenuBar;
	};
}