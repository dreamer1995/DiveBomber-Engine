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
		virtual [[nodiscard]] std::string GetCaption() const noexcept = 0;
		virtual [[nodiscard]] bool GetIsUniqueUI() const noexcept = 0;

		[[nodiscard]] UINT GetID() const noexcept
		{
			return id;
		}

		void SetID(UINT inputID) noexcept
		{
			id = inputID;
		}

		[[nodiscard]] bool IsShown() const noexcept
		{
			return isShown;
		}

	protected:
		UINT id = -1;
		bool isShown = true;
	};
}