#pragma once

namespace DiveBomber::UI
{
	class UIWidget
	{
	public:
		virtual ~UIWidget() = default;

		virtual void DrawUI() = 0;
	};
}