#pragma once
#include "Object.h"

namespace DiveBomber::DEObject
{
	class Prefab final : public Object
	{
	public:
		Prefab(const fs::path inputPath);

		void virtual DrawDetailPanel() override;
		void CreateConfig() override;
		void virtual SaveConfig() override;
		void UpdateConfig() noexcept;
	private:
	};
}