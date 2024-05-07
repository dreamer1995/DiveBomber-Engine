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
	}

	GUIDManager::~GUIDManager()
	{
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

		//json config;
		//config["GUID"] = Utility::utf8_encode(guidWString);

		//// write prettified JSON to another file
		//std::ofstream outFile(ProjectDirectoryW L"Cache\\GUID.json");
		//outFile << std::setw(4) << config << std::endl;
		//outFile.close();

		//guidWString = Utility::utf8_decode(config["GUID"]);

		//GUID newGuid = Utility::WStringToGUID(guidWString);

		//std::cout << IsEqualGUID(guid, newGuid);

		return guid;
	}
}
