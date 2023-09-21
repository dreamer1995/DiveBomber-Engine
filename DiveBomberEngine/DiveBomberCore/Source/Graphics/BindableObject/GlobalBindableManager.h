#pragma once
#include "..\..\Utility\Common.h"

#include <memory>
#include <cassert>
#include <unordered_map>
#include <mutex>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::BindableObject
{
	class Bindable;
}

namespace DiveBomber::BindableObject
{
	class GlobalBindableManager
	{
	public:
		GlobalBindableManager()
		{
		}

		GlobalBindableManager(const GlobalBindableManager&) = delete;
		GlobalBindableManager& operator =(const GlobalBindableManager&) = delete;

		template<class T, typename...Params>
		static std::shared_ptr<T> Resolve(DEGraphics::Graphics& gfx, Params&&...p) noxnd
		{
			static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
			return GetInstance().Resolve_<T>(gfx, std::forward<Params>(p)...);
		}

		static GlobalBindableManager& GetInstance()
		{
			static GlobalBindableManager globalBindableManager;
			return globalBindableManager;
		}

		void ClearPool() noexcept
		{
			binds.clear();
		}

	private:
		template<class T, typename...Params>
		std::shared_ptr<T> Resolve_(DEGraphics::Graphics& gfx, Params&&...p) noxnd
		{
			const auto key = T::GenerateUID(std::forward<Params>(p)...);
			const auto i = binds.find(key);

			bool needConstruct = true;
			if (i != binds.end())
			{
				if (i->second)
				{
					needConstruct = false;
				}
				else
				{
					binds.erase(i);
				}
			}

			if (needConstruct)
			{
				std::lock_guard<std::mutex> lock(globalBindableManagerMutex);
				auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
				binds[key] = bind;
				return bind;
			}
			else
			{
				return std::static_pointer_cast<T>(i->second);
			}
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<BindableObject::Bindable>> binds;

		std::mutex globalBindableManagerMutex;
	};
}
