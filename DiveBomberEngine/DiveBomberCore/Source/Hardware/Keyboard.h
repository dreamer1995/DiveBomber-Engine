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
			bool IsKeyDown() const noexcept
			{
				return type == Type::KBE_KeyDown;
			}
			bool IsKeyUp() const noexcept
			{
				return type == Type::KBE_KeyUp;
			}
			unsigned char GetCode() const noexcept
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
		bool KeyIsDown(unsigned char keycode) const noexcept;
		std::optional<Event> ReadKey() noexcept;
		bool KeyIsEmpty() const noexcept;
		void FlushKey() noexcept;
		// char event stuff
		std::optional<char> ReadChar() noexcept;
		bool CharIsEmpty() const noexcept;
		void FlushChar() noexcept;
		void Flush() noexcept;
		// autorepeat control
		void EnableAutorepeat() noexcept;
		void DisableAutorepeat() noexcept;
		bool AutorepeatIsEnabled() const noexcept;

		void OnKeyDown(unsigned char keycode) noexcept;
		void OnKeyUp(unsigned char keycode) noexcept;
		void OnChar(char character) noexcept;
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