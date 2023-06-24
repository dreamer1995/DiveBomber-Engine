#pragma once
#include "..\Utility\Common.h"
#include "..\Window\DEWindows.h"

#include <string>
#include <sstream>

namespace DiveBomber::DEException
{
	class Exception : public std::exception
	{
	public:
		Exception(int inputLine, const char* inputFile) noexcept;
		[[nodiscard]] virtual const wchar_t* whatW() const noexcept;
		[[nodiscard]] virtual const wchar_t* GetType() const noexcept;
		[[nodiscard]] int GetLine() const noexcept;
		[[nodiscard]] const std::wstring& GetFile() const noexcept;
		[[nodiscard]] std::wstring GetOriginString() const noexcept;
		[[nodiscard]] static std::wstring TranslateErrorCode(HRESULT hr) noexcept;
	private:
		int line;
		std::wstring file;
	protected:
		mutable std::wstring whatBuffer;
	};
}