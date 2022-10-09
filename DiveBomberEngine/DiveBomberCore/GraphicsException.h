// Include file line and translatecode to description, and additional with DXGI info
#pragma once
#include <Windows.h>
#include <vector>
#include "Exception.h"
#include "WRL.h"
#include "DxgiInfoManager.h"

// HRESULT hr should exist in the local scope for these macros to work

#define GFX_EXCEPT_NOINFO(hr) GraphicsHrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw GraphicsHrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) GraphicsHrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) GraphicsHrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// macro for importing infomanager into local scope
// this.GetInfoManager(Graphics& gfx) must exist
#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else
#define INFOMAN(gfx) HRESULT hr; DxgiInfoManager& infoManager = GetInfoManager((gfx))
#endif

#ifdef NDEBUG
#define INFOMAN_NOHR(gfx)
#else
#define INFOMAN_NOHR(gfx) DxgiInfoManager& infoManager = GetInfoManager((gfx))
#endif

class GraphicsHrException : public Exception
{
public:
	GraphicsHrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorInfo() const noexcept;
	std::string GetErrorDescription() const noexcept;
	static std::string TranslateErrorCode(HRESULT hr) noexcept;
private:
	HRESULT hr;
	std::string info;
};
class InfoException : public Exception
{
public:
	InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;
private:
	std::string info;
};
class DeviceRemovedException : public GraphicsHrException
{
	using GraphicsHrException::GraphicsHrException;
public:
	const char* GetType() const noexcept override;
private:
	std::string reason;
};

