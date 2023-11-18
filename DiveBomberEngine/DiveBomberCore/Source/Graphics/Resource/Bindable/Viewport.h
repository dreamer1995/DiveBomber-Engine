#pragma once
#include "..\Resource.h"
#include "Bindable.h"
#include "..\..\GraphicsHeader.h"

namespace DiveBomber::DEResource
{
	class Viewport final : public Resource, public Bindable
	{
	public:
		Viewport(const std::wstring& inputName);
		~Viewport();
		[[nodiscard]] D3D12_VIEWPORT GetViewport() const noexcept;
		void ResizeViewport(uint32_t width, uint32_t height) noxnd;
		void Bind() noxnd override;
	private:
		D3D12_VIEWPORT viewport;
	};
}