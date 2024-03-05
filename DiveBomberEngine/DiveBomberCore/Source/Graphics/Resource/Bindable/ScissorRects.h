#pragma once
#include "..\..\..\Resource.h"
#include "Bindable.h"
#include "..\..\GraphicsHeader.h"

namespace DiveBomber::DEResource
{
	class ScissorRects final : public DiveBomber::Resource, public Bindable
	{
	public:
		ScissorRects(const std::wstring& inputName);
		~ScissorRects();
		[[nodiscard]] D3D12_RECT GetScissorRects() const noexcept;
		void Bind() noxnd override;
	private:
		D3D12_RECT scissorRects;
	};
}