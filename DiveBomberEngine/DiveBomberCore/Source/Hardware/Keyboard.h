#pragma once

#include <queue>
#include <bitset>
#include <optional>

namespace DiveBomber::Hardware
{
	class Keyboard final
	{
	public:
		class Event
		{
		public:
			enum class Type
			{
				KBE_KeyDown,
				KBE_KeyUp,
			};
			Event(Type inputType, unsigned char inputCode) noexcept
			{
				type = inputType;
				code = inputCode;
			}
			[[nodiscard]] bool IsKeyDown() const noexcept
			{
				return type == Type::KBE_KeyDown;
			}
			[[nodiscard]] bool IsKeyUp() const noexcept
			{
				return type == Type::KBE_KeyUp;
			}
			[[nodiscard]] unsigned char GetCode() const noexcept
			{
				return code;
			}
		private:
			Type type;
			unsigned char code;
		};

		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;
		// key event stuff
		[[nodiscard]] bool KeyIsDown(const unsigned char keycode) const noexcept;
		std::optional<Event> ReadKey() noexcept;
		[[nodiscard]] bool KeyIsEmpty() const noexcept;
		void FlushKey() noexcept;
		// char event stuff
		std::optional<char> ReadChar() noexcept;
		[[nodiscard]] bool CharIsEmpty() const noexcept;
		void FlushChar() noexcept;
		void Flush() noexcept;
		// autorepeat control
		void EnableAutorepeat() noexcept;
		void DisableAutorepeat() noexcept;
		[[nodiscard]] bool AutorepeatIsEnabled() const noexcept;

		void OnKeyDown(const unsigned char keycode) noexcept;
		void OnKeyUp(const unsigned char keycode) noexcept;
		void OnChar(const char character) noexcept;
		void ClearState() noexcept;
	private:
		template<typename T>
		static void TrimBuffer(std::queue<T>& buffer) noexcept;

		static constexpr unsigned int nKeys = 256u;
		static constexpr unsigned int bufferSize = 16u;
		bool autorepeatEnabled = false;
		std::bitset<nKeys> keystates;
		std::queue<Event> keybuffer;
		std::queue<char> charbuffer;
	};
}