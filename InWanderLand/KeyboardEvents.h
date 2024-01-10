/// 2024. 01. 09 김상준
/// Editor Keyboard 용 이벤트들을 위한 클래스

#pragma once

#include "EditorEvents.h"

#include <sstream>

namespace application
{
	namespace editor
	{
		class KeyPressedEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Keyboard);
			EVENT_SETTING(EventType::KeyPressed);
		};

		class KeyReleasedEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Keyboard);
			EVENT_SETTING(EventType::KeyReleased);
		};
	}
}

