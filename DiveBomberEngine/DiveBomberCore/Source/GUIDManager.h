#pragma once
#include "Utility\DEJson.h"
#include "Utility\GlobalParameters.h"
#include "Utility\DEGUID.h"

#include <filesystem>
#include <fstream>

namespace DiveBomber
{
	using json = nlohmann::json;
	namespace fs = std::filesystem;

	class GUIDManager final
	{
	public:
		GUIDManager();
		~GUIDManager();

		[[nodiscard]] static GUIDManager& GetInstance();
		static void Destructor() noexcept;

		[[nodiscard]] static GUID GenerateGUID() noexcept;

	private:
		static std::unique_ptr<GUIDManager> instance;
		static std::map<std::string, fs::path> guidMap;
	};
}
