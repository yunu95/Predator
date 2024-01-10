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
		};

		class KeyPressedEvent
			: public KeyboardEvent
		{
		public:
			EVENT_SETTING(EventType::KeyPressed)
		};

		class KeyDownEvent
			: public KeyboardEvent
		{
		public:
			EVENT_SETTING(EventType::KeyDown)
		};

		class KeyReleasedEvent
			: public KeyboardEvent
		{
		public:
			EVENT_SETTING(EventType::KeyReleased)
		};
	}
}

