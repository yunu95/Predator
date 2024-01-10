/// 2024. 01. 09 김상준
/// Editor Mouse 입력 이벤트들을 위한 클래스

#pragma once

#include "EditorEvents.h"

#include <sstream>

namespace application
{
	namespace editor
	{
		class MouseButtonPressedEvent
			: public EditorEvents
		{
		public:
			EVENT_SETTING(Mouse, MouseButtonPressed);
		};

		class MouseButtonReleasedEvent
			: public EditorEvents
		{
		public:
			EVENT_SETTING(Mouse, MouseButtonReleased);
		};

		class MouseButtonDownEvent
			: public EditorEvents
		{
		public:
			EVENT_SETTING(Mouse, MouseButtonDown);
		};

		class MouseMovedEvent
			: public EditorEvents
		{
		public:
			EVENT_SETTING(Mouse, MouseMoved);
		};

		class MouseScrolledEvent
			: public EditorEvents
		{
		public:
			EVENT_SETTING(Mouse, MouseScrolled);
		};
	}
}

