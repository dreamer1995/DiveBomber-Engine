// Include file line and translatecode to description
#pragma once
#include "Exception.h"

namespace DiveBomber::DEException
{
#define WND_EXCEPT( hr ) WindowHrException( __LINE__,__FILE__,(hr) )
#define WND_LAST_EXCEPT() WindowHrException( __LINE__,__FILE__,GetLastError() )
#define WND_NOGFX_EXCEPT() NoGfxException( __LINE__,__FILE__ )

	class WindowHrException final : public Exception
	{
	public:
		WindowHrException(int line, const char* file, HRESULT inputHr) noexcept;
		[[nodiscard]] const wchar_t* whatW() const noexcept override;
		[[nodiscard]] const wchar_t* GetType() const noexcept override;
		[[nodiscard]] HRESULT GetErrorCode() const noexcept;
		[[nodiscard]] std::wstring GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		[[nodiscard]] const wchar_t* GetType() const noexcept override;
	};
}