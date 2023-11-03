#pragma once

#include "..\..\Window\DEWindows.h"

namespace DiveBomber::BindableObject
{
	class BindableShaderInput
	{
	public:
		virtual ~BindableShaderInput() = default;
		[[nodiscard]] virtual UINT GetSRVDescriptorHeapOffset() const noexcept = 0;
	};
}
