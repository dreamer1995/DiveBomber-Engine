#pragma once

namespace DiveBomber
{
	class ConfigDrivenResource
	{
	public:
		virtual void DrawDetailPanel() = 0;
		virtual void SaveConfig() = 0;
	};
}
