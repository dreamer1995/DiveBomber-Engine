#include "Renderer.h"

namespace DiveBomber::DEComponent
{
	Renderer::Renderer(const std::wstring inputName)
		:
		Component(inputName),
		ConfigDrivenResource(L"")
	{
	}
}