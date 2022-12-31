#pragma once
#include <vector>
#include <dxgidebug.h>
#include <string>

#include "WRL.h"
#include "WindowResource.h"
#include "WindowException.h"

class DxgiInfoManager final
{
public:
	static DxgiInfoManager* GetInstance() noexcept;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	DxgiInfoManager();
	~DxgiInfoManager();
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
	static DxgiInfoManager* g_pDxgiInfoMng;
};