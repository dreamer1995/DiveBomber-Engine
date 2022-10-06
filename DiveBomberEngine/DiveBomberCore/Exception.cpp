#include "Exception.h"

Exception::Exception(int inputLine, const char* inputFile) noexcept
{
	line = inputLine;
	file = inputFile;
}

const char* Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Exception::GetType() const noexcept
{
	return "Standard Exception";
}

int Exception::GetLine() const noexcept
{
	return line;
}

const std::string&  Exception::GetFile() const noexcept
{
	return file;
}

std::string Exception::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}