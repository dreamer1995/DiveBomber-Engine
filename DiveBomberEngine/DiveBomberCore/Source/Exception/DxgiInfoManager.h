#pragma once
#include "..\Graphics\DEWrl.h"

#include <vector>
#include <dxgidebug.h>
#include <string>

namespace DiveBomber::DEException
{
	class DxgiInfoManager final
	{
	public:
		[[nodiscard]] static DxgiInfoManager* GetInstance() noexcept;
		void Set() noexcept;
		[[nodiscard]] std::vector<std::string> GetMessages() const;
	private:
		DxgiInfoManager();
		~DxgiInfoManager();
		DxgiInfoManager(const DxgiInfoManager&) = delete;
		DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
		unsigned long long next = 0u;
		Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
		static DxgiInfoManager* g_pDxgiInfoMng;
	};
}