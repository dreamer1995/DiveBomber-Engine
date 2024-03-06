#pragma once

namespace DiveBomber::UI
{
	class DetailModifier
	{
	public:
		virtual void DrawDetailPanel() = 0;
		virtual void SaveConfig() = 0;
	};
}
