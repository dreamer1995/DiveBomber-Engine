#pragma once

#include <queue>
#include <optional>

namespace DiveBomber::Hardware
{
	class Mouse final
	{
	public:
		struct RawDelta
		{
			int x, y;
		};
		class Event
		{
		public:
			enum class Type
			{
				MBE_LDown,
				MBE_LUp,
				MBE_RDown,
				MBE_RUp,
				MBE_WheelFront,
				MBE_WheelBack,
				MBE_WheelDown,
				MBE_WheelUp,
				MBE_Move,
				MBE_Enter,
				MBE_Leave,
			};
		private:
			Type type;
			bool leftIsDown;
			bool rightIsDown;
			bool wheelIsDown;
			int x;
			int y;
		public:
			Event(Type inputType, const Mouse& parent) noexcept
			{
				type = inputType;
				leftIsDown = parent.leftIsDown;
				rightIsDown = parent.rightIsDown;
				wheelIsDown = parent.rightIsDown;
				x = parent.x;
				y = parent.y;
			}
			[[nodiscard]] Type GetType() const noexcept
			{
				return type;
			}
			[[nodiscard]] std::pair<int, int> GetPos() const noexcept
			{
				return{ x,y };
			}
			[[nodiscard]] int GetPosX() const noexcept
			{
				return x;
			}
			[[nodiscard]] int GetPosY() const noexcept
			{
				return y;
			}
			[[nodiscard]] bool LeftIsPressed() const noexcept
			{
				return leftIsDown;
			}
			[[nodiscard]] bool RightIsPressed() const noexcept
			{
				return rightIsDown;
			}
			[[nodiscard]] bool WheelIsPressed() const noexcept
			{
				return wheelIsDown;
			}
		};
	public:
		Mouse() = default;
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;
		std::pair<int, int> GetPos() const noexcept;
		std::optional<RawDelta> ReadRawDelta() noexcept;
		[[nodiscard]] int GetPosX() const noexcept;
		[[nodiscard]] int GetPosY() const noexcept;
		[[nodiscard]] bool IsInWindow() const noexcept;
		[[nodiscard]] bool LeftIsDown() const noexcept;
		[[nodiscard]] bool RightIsDown() const noexcept;
		[[nodiscard]] bool WheelIsDown() const noexcept;
		[[nodiscard]] std::optional<Mouse::Event> Read() noexcept;
		[[nodiscard]] bool IsEmpty() const noexcept
		{
			return buffer.empty();
		}
		void Flush() noexcept;
		void EnableRaw() noexcept;
		void DisableRaw() noexcept;
		[[nodiscard]] bool RawEnabled() const noexcept;

		void OnMouseMove(int x, int y) noexcept;
		void OnMouseLeave() noexcept;
		void OnMouseEnter() noexcept;
		void OnRawDelta(int dx, int dy) noexcept;
		void OnLeftDown() noexcept;
		void OnLeftUp() noexcept;
		void OnRightDown() noexcept;
		void OnRightUp() noexcept;
		void OnWheelFront() noexcept;
		void OnWheelBack() noexcept;
		void OnWheelDelta(const int delta) noexcept;
		void OnWheelDown() noexcept;
		void OnWheelUp() noexcept;
	private:
		void TrimBuffer() noexcept;
		void TrimRawInputBuffer() noexcept;
	private:
		static constexpr unsigned int bufferSize = 16u;
		int x = 0;
		int y = 0;
		bool leftIsDown = false;
		bool rightIsDown = false;
		bool wheelIsDown = false;
		bool isInWindow = false;
		int wheelDeltaCarry = 0;
		bool rawEnabled = false;
		std::queue<Event> buffer;
		std::queue<RawDelta> rawDeltaBuffer;
	};
}
