#include "GlobalBindableManager.h"

namespace DiveBomber::BindableObject
{
	std::unique_ptr<GlobalBindableManager> GlobalBindableManager::instance;

	void GlobalBindableManager::DeleteBindable(const std::string key) noexcept
	{
		auto it = binds.find(key);
		if (it != binds.end())
		{
			binds.erase(it);
		}
	}

	GlobalBindableManager& GlobalBindableManager::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = std::make_unique<GlobalBindableManager>();
		}
		return *instance;
	}

	void GlobalBindableManager::Destructor() noexcept
	{
		if (instance != nullptr)
		{
			instance.reset();
		}
	}
}
