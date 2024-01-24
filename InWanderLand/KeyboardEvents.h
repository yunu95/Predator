/// 2024. 01. 09 김상준
/// Editor Keyboard 용 이벤트들을 위한 클래스

#pragma once

#include "EditorEvents.h"

#include <sstream>

namespace application
{
	namespace editor
	{
		class KeyboardEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Editor | EventCategory::Input | EventCategory::Keyboard)

			KeyCode GetKeyCode() const { return keyCode; }

		protected:
			KeyboardEvent(KeyCode keyCode)
				: keyCode(keyCode)
			{

			}

		private:
			KeyCode keyCode;
		};

		class KeyPressedEvent
			: public KeyboardEvent
		{
		public:
			EVENT_SETTING(EventType::KeyPressed)

			KeyPressedEvent(KeyCode keyCode)
				: KeyboardEvent(keyCode)
			{

			}

			virtual std::string GetDebugString() const
			{
				std::stringstream ss;
				ss << "[" + GetName() + "] ";
				auto code = GetKeyCode();
				ss << static_cast<unsigned int>(code) << '\n';
				return ss.str();
			}
		};

		class KeyDownEvent
			: public KeyboardEvent
		{
		public:
			EVENT_SETTING(EventType::KeyDown)

			KeyDownEvent(KeyCode keyCode)
				: KeyboardEvent(keyCode)
			{

			}

			virtual std::string GetDebugString() const
			{
				std::stringstream ss;
				ss << "[" + GetName() + "] ";
				auto code = GetKeyCode();
				ss << static_cast<unsigned int>(code) << '\n';
				return ss.str();
			}
		};

		class KeyReleasedEvent
			: public KeyboardEvent
		{
		public:
			EVENT_SETTING(EventType::KeyReleased)

			KeyReleasedEvent(KeyCode keyCode)
				: KeyboardEvent(keyCode)
			{

			}

			virtual std::string GetDebugString() const
			{
				std::stringstream ss;
				ss << "[" + GetName() + "] ";
				auto code = GetKeyCode();
				ss << static_cast<unsigned int>(code) << '\n';
				return ss.str();
			}
		};
	}
}
