#pragma once
#include "GUIDManager.h"
#include "Exception\WindowException.h"
#include "Utility\Common.h"

namespace DiveBomber
{
	using namespace DEException;
	std::unique_ptr<GUIDManager> GUIDManager::instance;

	GUIDManager::GUIDManager()
	{
		LoadGUIDMap();
	}

	GUIDManager& GUIDManager::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = std::make_unique<GUIDManager>();
		}
		return *instance;
	}

	void GUIDManager::Destructor() noexcept
	{
		if (instance != nullptr)
		{
			instance.reset();
		}
	}

	GUID GUIDManager::GenerateGUID() noexcept
	{
		GUID guid;
		WND_EXCEPT(CoCreateGuid(&guid));

		std::wstring guidWString = Utility::GUIDToWString(guid);
		//std::cout << IsEqualGUID(guid, newGuid);

		return guid;
	}

	void GUIDManager::LoadGUIDMap()
	{
		json guidMapJson;

		if (fs::exists(guidMapPath))
		{
			std::ifstream rawFile(guidMapPath);
			if (!rawFile.is_open())
			{
				throw std::exception(std::format("Unable to open database file {}", guidMapPath.string()).c_str());
			}
			rawFile >> guidMapJson;
			rawFile.close();

			for (auto it = guidMapJson.begin(); it != guidMapJson.end(); it++)
			{
				guidMap[Utility::WStringToGUID(Utility::utf8_decode(it.key()))] = Utility::utf8_decode(it.value());
			}
		}
		else
		{
			RebuildGUIDMap();
		}
	}

	void GUIDManager::SaveGUIDMap() noexcept
	{
		json guidMapJson;

		for (auto& guidData : guidMap)
		{
			guidMapJson[Utility::utf8_encode(Utility::GUIDToWString(guidData.first))] = Utility::utf8_encode(fs::relative(guidData.second));
		}

		// write prettified JSON to another file
		std::ofstream outFile(guidMapPath);
		outFile << std::setw(4) << guidMapJson << std::endl;
		outFile.close();
	}

	void GUIDManager::RebuildGUIDMap(fs::path path) noexcept
	{
		if (path != L"")
		{
			RecursiveFilePath(path);
		}
		else
		{
			RecursiveFilePath(L"G:\\repo\\DiveBomber-Engine\\SampleProject");
			RecursiveFilePath(L"G:\\repo\\DiveBomber-Engine\\DiveBomberEngine\\DiveBomberCore");
		}
	}

	fs::path GUIDManager::GUIDToPath(GUID guid) noexcept
	{
		fs::path path = guidMap[guid];
		assert((path != L"") && "GUID has no valid path-mapping");
		return path;
	}

	void GUIDManager::RecursiveFilePath(fs::path path)
	{
		for (auto const& dir_entry : fs::directory_iterator(path))
		{
			if (dir_entry.is_directory())
			{
				RecursiveFilePath(dir_entry);
			}
			else if (dir_entry.path().extension() == L".deasset")
			{
				json config;

				std::ifstream rawFile(dir_entry.path());
				rawFile >> config;
				rawFile.close();

				auto it = config.find("GUID");
				if (it != config.end())
				{
					std::wstring guidWString = Utility::utf8_decode(config["GUID"]);

					GUID guid = Utility::WStringToGUID(guidWString);

					guidMap[guid] = dir_entry.path();
				}
				else
				{
					throw std::exception(std::format("There is no \"GUID\" key in {}", dir_entry.path().string()).c_str());
				}
			}
		}

		SaveGUIDMap();
	}
}
