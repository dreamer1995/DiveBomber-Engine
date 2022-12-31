#pragma once
#include <queue>
#include <optional>

class Mouse final
{
	friend class Window;
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
		Type GetType() const noexcept
		{
			return type;
		}
		std::pair<int, int> GetPos() const noexcept
		{
			return{ x,y };
		}
		int GetPosX() const noexcept
		{
			return x;
		}
		int GetPosY() const noexcept
		{
			return y;
		}
		bool LeftIsPressed() const noexcept
		{
			return leftIsDown;
		}
		bool RightIsPressed() const noexcept
		{
			return rightIsDown;
		}
		bool WheelIsPressed() const noexcept
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
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsDown() const noexcept;
	bool RightIsDown() const noexcept;
	bool WheelIsDown() const noexcept;
	std::optional<Mouse::Event> Read() noexcept;
	bool IsEmpty() const noexcept
	{
		return buffer.empty();
	}
	void Flush() noexcept;
	void EnableRaw() noexcept;
	void DisableRaw() noexcept;
	bool RawEnabled() const noexcept;
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnRawDelta(int dx, int dy) noexcept;
	void OnLeftDown(int x, int y) noexcept;
	void OnLeftUp(int x, int y) noexcept;
	void OnRightDown(int x, int y) noexcept;
	void OnRightUp(int x, int y) noexcept;
	void OnWheelFront(int x, int y) noexcept;
	void OnWheelBack(int x, int y) noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void TrimBuffer() noexcept;
	void TrimRawInputBuffer() noexcept;
private:
	static constexpr unsigned int bufferSize = 16u;
	int x;
	int y;
	bool leftIsDown = false;
	bool rightIsDown = false;
	bool wheelIsDown = false;
	bool isInWindow = false;
	int wheelDeltaCarry = 0;
	bool rawEnabled = false;
	std::queue<Event> buffer;
	std::queue<RawDelta> rawDeltaBuffer;
};

