#pragma once
#include "Utility\DEJson.h"
#include "Utility\GlobalParameters.h"

#include <filesystem>
#include <fstream>

namespace DiveBomber
{
	using json = nlohmann::json;
	namespace fs = std::filesystem;

	class ConfigDrivenResource
	{
	public:
		ConfigDrivenResource(fs::path inputPath)
			:
			configFilePath(inputPath)
		{
		}

		virtual ~ConfigDrivenResource()
		{
		}

		virtual void DrawDetailPanel() = 0;
		virtual void SaveConfig() = 0;
		virtual void CreateConfig() = 0;

		[[nodiscard]]void ReadConfig(fs::path configFileCachePath)
		{
#if EditorMode
			if (!fs::exists(configFilePath))
			{
				if (!fs::exists(configFilePath.parent_path()))
				{
					fs::create_directories(configFilePath.parent_path());
				}

				CreateConfig();
				configFileCachePath = configFilePath;
			}
			else
			{
				// if no cache or cache obsoleted
				if (!fs::exists(configFileCachePath) || fs::last_write_time(configFileCachePath) < fs::last_write_time(configFilePath))
				{
					if (!fs::exists(configFileCachePath.parent_path()))
					{
						fs::create_directories(configFileCachePath.parent_path());
					}
					fs::copy(configFilePath, configFileCachePath, fs::copy_options::update_existing);
				}
			}
#endif // EditorMode

			std::ifstream rawFile(configFileCachePath);
			if (!rawFile.is_open())
			{
				throw std::exception(std::format("Unable to open config file {}", configFileCachePath.string()).c_str());
			}
			rawFile >> config;
		}

	protected:
		fs::path configFilePath;
		json config;
	};
}
