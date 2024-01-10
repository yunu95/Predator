/// 2024. 01. 09 김상준
/// Editor Mouse 입력 이벤트들을 위한 클래스

#pragma once

#include "EditorEvents.h"

#include <sstream>

namespace application
{
	namespace editor
	{
		class MouseButtonEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Mouse);
		};

		class MouseButtonPressedEvent
			: public MouseButtonEvent
		{
		public:
			EVENT_SETTING(EventType::MouseButtonPressed);
		};

		class MouseButtonReleasedEvent
			: public MouseButtonEvent
		{
		public:
			EVENT_SETTING(EventType::MouseButtonReleased);
		};

		class MouseButtonDownEvent
			: public MouseButtonEvent
		{
		public:
			EVENT_SETTING(EventType::MouseButtonDown);
		};

		class MouseMovedEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Mouse);
			EVENT_SETTING(EventType::MouseMoved);
		};

		class MouseScrolledEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Mouse);
			EVENT_SETTING(EventType::MouseScrolled);
		};
	}
}

