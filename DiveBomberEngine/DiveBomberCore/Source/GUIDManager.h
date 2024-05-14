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
		GUIDManager(const GUIDManager&) = delete;
		GUIDManager& operator =(const GUIDManager&) = delete;

		static GUIDManager& GetInstance();
		static void Destructor() noexcept;

		[[nodiscard]] static GUID GenerateGUID() noexcept;
		void RebuildGUIDMap(fs::path path = L"") noexcept;

		[[nodiscard]] fs::path GUIDToPath(GUID guid) noexcept;

	private:
		void LoadGUIDMap();
		void SaveGUIDMap() noexcept;
		void RecursiveFilePath(fs::path path);

	private:
		static std::unique_ptr<GUIDManager> instance;
		std::map<GUID, fs::path> guidMap;
		fs::path guidMapPath = ProjectDirectoryW L"Cache\\GUIDMap.dedatabase";
	};
}
