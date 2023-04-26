#include "Mouse.h"

#include "..\Window\DEWindows.h"

namespace DiveBomber::Hardware
{
	std::pair<int, int> Mouse::GetPos() const noexcept
	{
		return { x,y };
	}

	std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
	{
		if (rawDeltaBuffer.empty())
		{
			return std::nullopt;
		}
		const RawDelta d = rawDeltaBuffer.front();
		rawDeltaBuffer.pop();
		return d;
	}

	int Mouse::GetPosX() const noexcept
	{
		return x;
	}

	int Mouse::GetPosY() const noexcept
	{
		return y;
	}

	bool Mouse::IsInWindow() const noexcept
	{
		return isInWindow;
	}

	bool Mouse::LeftIsDown() const noexcept
	{
		return leftIsDown;
	}

	bool Mouse::RightIsDown() const noexcept
	{
		return rightIsDown;
	}

	bool Mouse::WheelIsDown() const noexcept
	{
		return wheelIsDown;
	}

	std::optional<Mouse::Event> Mouse::Read() noexcept
	{
		if (buffer.size() > 0u)
		{
			Mouse::Event e = buffer.front();
			buffer.pop();
			return e;
		}
		return {};
	}

	void Mouse::Flush() noexcept
	{
		buffer = std::queue<Event>();
	}

	void Mouse::EnableRaw() noexcept
	{
		rawEnabled = true;
	}

	void Mouse::DisableRaw() noexcept
	{
		rawEnabled = false;
	}

	bool Mouse::RawEnabled() const noexcept
	{
		return rawEnabled;
	}

	void Mouse::OnMouseMove(int newx, int newy) noexcept
	{
		x = newx;
		y = newy;

		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_Move, *this));
		TrimBuffer();
	}

	void Mouse::OnMouseLeave() noexcept
	{
		isInWindow = false;
		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_Leave, *this));
		TrimBuffer();
	}

	void Mouse::OnMouseEnter() noexcept
	{
		isInWindow = true;
		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_Enter, *this));
		TrimBuffer();
	}

	void Mouse::OnRawDelta(int dx, int dy) noexcept
	{
		rawDeltaBuffer.push({ dx,dy });
		TrimBuffer();
	}

	void Mouse::OnLeftDown(int x, int y) noexcept
	{
		leftIsDown = true;

		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_LDown, *this));
		TrimBuffer();
	}

	void Mouse::OnLeftUp(int x, int y) noexcept
	{
		leftIsDown = false;

		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_LUp, *this));
		TrimBuffer();
	}

	void Mouse::OnRightDown(int x, int y) noexcept
	{
		rightIsDown = true;

		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_RDown, *this));
		TrimBuffer();
	}

	void Mouse::OnRightUp(int x, int y) noexcept
	{
		rightIsDown = false;

		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_RUp, *this));
		TrimBuffer();
	}

	void Mouse::OnWheelFront(int x, int y) noexcept
	{
		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_WheelFront, *this));
		TrimBuffer();
	}

	void Mouse::OnWheelBack(int x, int y) noexcept
	{
		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_WheelBack, *this));
		TrimBuffer();
	}

	void Mouse::OnWheelDown(int x, int y) noexcept
	{
		wheelIsDown = true;

		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_WheelDown, *this));
		TrimBuffer();
	}

	void Mouse::OnWheelUp(int x, int y) noexcept
	{
		wheelIsDown = false;

		buffer.push(Mouse::Event(Mouse::Event::Type::MBE_WheelUp, *this));
		TrimBuffer();
	}

	void Mouse::TrimBuffer() noexcept
	{
		while (buffer.size() > bufferSize)
		{
			buffer.pop();
		}
	}

	void Mouse::TrimRawInputBuffer() noexcept
	{
		while (rawDeltaBuffer.size() > bufferSize)
		{
			rawDeltaBuffer.pop();
		}
	}

	void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
	{
		wheelDeltaCarry += delta;
		// generate events for every 120 
		while (wheelDeltaCarry >= WHEEL_DELTA)
		{
			wheelDeltaCarry -= WHEEL_DELTA;
			OnWheelFront(x, y);
		}
		while (wheelDeltaCarry <= -WHEEL_DELTA)
		{
			wheelDeltaCarry += WHEEL_DELTA;
			OnWheelBack(x, y);
		}
	}
}