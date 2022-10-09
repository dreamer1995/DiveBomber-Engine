// Include file line and translatecode to description
#pragma once
#include <Windows.h>
#include "Exception.h"

#define WND_EXCEPT( hr ) WindowHrException( __LINE__,__FILE__,(hr) )
#define WND_LAST_EXCEPT() WindowHrException( __LINE__,__FILE__,GetLastError() )
#define WND_NOGFX_EXCEPT() NoGfxException( __LINE__,__FILE__ )

class WindowException : public Exception
{
	using Exception::Exception;
public:
	static std::string TranslateErrorCode(HRESULT hr) noexcept;
};
class WindowHrException : public WindowException
{
public:
	WindowHrException(int line, const char* file, HRESULT hr) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorDescription() const noexcept;
private:
	HRESULT hr;
};
class NoGfxException : public WindowException
{
public:
	using WindowException::WindowException;
	const char* GetType() const noexcept override;
};

