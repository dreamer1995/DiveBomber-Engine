#pragma once
#include "..\..\Utility\Common.h"

#include <string>
#include <memory>
#include <cassert>

namespace DiveBomber::DEResource
{
	class Resource
	{
	public:
		Resource(std::wstring inputName)
			:
			name(inputName)
		{
		}

		virtual ~Resource() = default;

		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::string& name, Ignore&&...ignore)
		{
			using namespace std::string_literals;
			return typeid(Resource).name() + "#"s + name;
		}

		[[nodiscard]] virtual std::string GetUID() const noexcept
		{
			return GenerateUID(Utility::ToNarrow(name));
		}

	protected:
		std::wstring name;
	};
}
