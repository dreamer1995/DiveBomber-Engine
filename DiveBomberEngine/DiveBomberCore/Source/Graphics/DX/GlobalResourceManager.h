#pragma once
#include "..\..\Utility\Common.h"

#include <memory>
#include <cassert>
#include <unordered_map>
#include <mutex>

namespace DiveBomber::DEResource
{
	class Resource;
}

namespace DiveBomber::DX
{
	class GlobalResourceManager final
	{
	public:
		GlobalResourceManager()
		{
		}

		GlobalResourceManager(const GlobalResourceManager&) = delete;
		GlobalResourceManager& operator =(const GlobalResourceManager&) = delete;

		void DeleteResource(const std::string key) noexcept;

		template<class T, typename...Params>
		[[nodiscard]] static std::shared_ptr<T> Resolve(Params&&...p) noxnd
		{
			static_assert(std::is_base_of<DEResource::Resource, T>::value, "Can only resolve classes derived from Resource");
			return GetInstance().Resolve_<T>(std::forward<Params>(p)...);
		}

		[[nodiscard]] static GlobalResourceManager& GetInstance();
		static void Destructor() noexcept;

	private:
		template<class T, typename...Params>
		std::shared_ptr<T> Resolve_(Params&&...p) noxnd
		{
			const auto key = T::GenerateUID(p...);
			const auto i = resources.find(key);

			bool needConstruct = true;
			if (i != resources.end())
			{
				if (i->second)
				{
					needConstruct = false;
				}
				else
				{
					resources.erase(i);
				}
			}

			if (needConstruct)
			{
				auto bind = std::make_shared<T>(std::forward<Params>(p)...);
				GlobalResourceManagerMutex.lock();
				resources[key] = bind;
				GlobalResourceManagerMutex.unlock();
				return bind;
			}
			else
			{
				return std::static_pointer_cast<T>(i->second);
			}
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<DEResource::Resource>> resources;

		std::mutex GlobalResourceManagerMutex;

		static std::unique_ptr<GlobalResourceManager> instance;
	};
}
