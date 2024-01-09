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
			EVENT_SETTING(Keyboard, KeyPressed);
		};

		class KeyReleasedEvent
			: public EditorEvents
		{
		public:
			EVENT_SETTING(Keyboard, KeyReleased);
		};
	}
}

