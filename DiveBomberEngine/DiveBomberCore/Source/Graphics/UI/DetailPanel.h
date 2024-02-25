#pragma once
#include "UIWidget.h"

namespace DiveBomber::UI
{
	class DetailPanel final : public UIWidget
	{
	public:
		void DrawUI() override;
		[[nodiscard]] std::string GetCaption() const noexcept override
		{
			return "Detail";
		}
		[[nodiscard]] bool GetIsUniqueUI() const noexcept override
		{
			return false;
		}
	};
}