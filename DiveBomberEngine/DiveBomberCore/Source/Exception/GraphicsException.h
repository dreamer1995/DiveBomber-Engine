// Include file line and translatecode to description, and additional with DXGI info
#pragma once
#include "Exception.h"
#include "DxgiInfoManager.h"

namespace DiveBomber::DEException
{
// HRESULT hr should exist in the local scope for these macros to work
#define GFX_EXCEPT_NOINFO(hr) GraphicsHrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw GraphicsHrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) GraphicsHrException( __LINE__,__FILE__,(hr),DxgiInfoManager::GetInstance()->GetMessages() )
#define GFX_THROW_INFO(hrcall) DxgiInfoManager::GetInstance()->Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException( __LINE__,__FILE__,(hr),DxgiInfoManager::GetInstance()->GetMessages() )
#define GFX_THROW_INFO_ONLY(call) DxgiInfoManager::GetInstance()->Set(); (call); {auto v = DxgiInfoManager::GetInstance()->GetMessages(); if(!v.empty()) {throw InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) GraphicsHrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// macro for importing infomanager into local scope
// this.GetInfoManager(Graphics& gfx) must exist
//#ifdef NDEBUG
//#define INFOMAN(gfx) HRESULT hr
//#else
//#define INFOMAN(gfx) HRESULT hr; DxgiInfoManager& infoManager = GetInfoManager((gfx))
//#endif
//
//#ifdef NDEBUG
//#define INFOMAN_NOHR(gfx)
//#else
//#define INFOMAN_NOHR(gfx) DxgiInfoManager& infoManager = GetInfoManager((gfx))
//#endif

	class GraphicsHrException : public Exception
	{
	public:
		GraphicsHrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const wchar_t* whatW() const noexcept override;
		const wchar_t* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::wstring GetErrorInfo() const noexcept;
		std::wstring GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
		std::wstring info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const wchar_t* whatW() const noexcept override;
		const wchar_t* GetType() const noexcept override;
		std::wstring GetErrorInfo() const noexcept;
	private:
		std::wstring info;
	};
	class DeviceRemovedException final : public GraphicsHrException
	{
		using GraphicsHrException::GraphicsHrException;
	public:
		const wchar_t* GetType() const noexcept override;
	private:
		std::wstring reason;
	};
}