#pragma once
#include "UIWidget.h"

namespace DiveBomber::UI
{
	class RenderStatusPanel final : public UIWidget
	{
	public:
		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() const noexcept override
		{
			return "Render Status";
		}
		[[nodiscard]] bool GetIsUniqueUI() const noexcept override
		{
			return true;
		}
	};
}