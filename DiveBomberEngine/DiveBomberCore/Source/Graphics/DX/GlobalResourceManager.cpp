#include "GlobalResourceManager.h"

namespace DiveBomber::DX
{
	using namespace DEResource;
	std::unique_ptr<GlobalResourceManager> GlobalResourceManager::instance;

	void GlobalResourceManager::DeleteResource(const std::string key) noexcept
	{
		auto it = resources.find(key);
		if (it != resources.end())
		{
			resources.erase(it);
		}
	}

	GlobalResourceManager& GlobalResourceManager::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = std::make_unique<GlobalResourceManager>();
		}
		return *instance;
	}

	void GlobalResourceManager::Destructor() noexcept
	{
		if (instance != nullptr)
		{
			instance.reset();
		}
	}
}
