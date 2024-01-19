#pragma once

#include "..\..\Window\DEWindows.h"

#include <string>

namespace DiveBomber::UI
{
	class UIWidget
	{
	public:
		virtual ~UIWidget() = default;

		virtual void DrawUI() = 0;
		virtual [[nodiscard]] std::string GetCaption() = 0;
		virtual [[nodiscard]] bool GetIsUniqueUI() = 0;

		[[nodiscard]] UINT GetID()
		{
			return id;
		}

		void SetID(UINT inputID)
		{
			id = inputID;
		}

		[[nodiscard]] bool IsShown()
		{
			return isShown;
		}

	protected:
		UINT id = -1;
		bool isShown = true;
	};
}