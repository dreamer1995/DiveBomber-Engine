#pragma once
#include "..\..\..\Window\DEWindows.h"

namespace DiveBomber::DEResource
{
	class ShaderInputable
	{
	public:

		virtual ~ShaderInputable() = default;

		[[nodiscard]] virtual UINT GetSRVDescriptorHeapOffset() const noexcept = 0;

		[[nodiscard]] virtual void BindAsShaderResource() noxnd
		{
		}
	};
}
