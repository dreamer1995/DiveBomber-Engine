#include "Keyboard.h"

namespace DiveBomber::Hardware
{
	bool Keyboard::KeyIsDown(const unsigned char keycode) const noexcept
	{
		return keystates[keycode];
	}

	std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
	{
		if (keybuffer.size() > 0u)
		{
			Keyboard::Event e = keybuffer.front();
			keybuffer.pop();
			return e;
		}
		return {};
	}

	bool Keyboard::KeyIsEmpty() const noexcept
	{
		return keybuffer.empty();
	}

	std::optional<char> Keyboard::ReadChar() noexcept
	{
		if (charbuffer.size() > 0u)
		{
			unsigned char charcode = charbuffer.front();
			charbuffer.pop();
			return charcode;
		}
		return {};
	}

	bool Keyboard::CharIsEmpty() const noexcept
	{
		return charbuffer.empty();
	}

	void Keyboard::FlushKey() noexcept
	{
		keybuffer = std::queue<Event>();
	}

	void Keyboard::FlushChar() noexcept
	{
		charbuffer = std::queue<char>();
	}

	void Keyboard::Flush() noexcept
	{
		FlushKey();
		FlushChar();
	}

	void Keyboard::EnableAutorepeat() noexcept
	{
		autorepeatEnabled = true;
	}

	void Keyboard::DisableAutorepeat() noexcept
	{
		autorepeatEnabled = false;
	}

	bool Keyboard::AutorepeatIsEnabled() const noexcept
	{
		return autorepeatEnabled;
	}

	void Keyboard::OnKeyDown(const unsigned char keycode) noexcept
	{
		keystates[keycode] = true;
		keybuffer.push(Keyboard::Event(Keyboard::Event::Type::KBE_KeyDown, keycode));
		TrimBuffer(keybuffer);
	}

	void Keyboard::OnKeyUp(const unsigned char keycode) noexcept
	{
		keystates[keycode] = false;
		keybuffer.push(Keyboard::Event(Keyboard::Event::Type::KBE_KeyUp, keycode));
		TrimBuffer(keybuffer);
	}

	void Keyboard::OnChar(const char character) noexcept
	{
		charbuffer.push(character);
		TrimBuffer(charbuffer);
	}

	void Keyboard::ClearState() noexcept
	{
		keystates.reset();
	}

	template<typename T>
	void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
	{
		while (buffer.size() > bufferSize)
		{
			buffer.pop();
		}
	}
}