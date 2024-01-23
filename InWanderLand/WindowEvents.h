/// 2024. 01. 09 김상준
/// Editor Window 용 이벤트들을 위한 클래스

#pragma once

#include "EditorEvents.h"

#include <sstream>

namespace application
{
	namespace editor
	{
		class WindowResizeEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Editor | EventCategory::Window)
			EVENT_SETTING(EventType::WindowResize)

			WindowResizeEvent(unsigned int width, unsigned int height)
				: width(width), height(height)
			{

			}

			virtual std::string GetDebugString() const
			{
				std::stringstream ss;
				ss << "[" + GetName() + "] width : " << width << " / height : " << height << '\n';
				return ss.str();
			}

		private:
			unsigned int width;
			unsigned int height;
		};

		class WindowFocusEvent
			: public EditorEvents
		{
			CATEGORY_SETTING(EventCategory::Editor | EventCategory::Window)
			EVENT_SETTING(EventType::WindowFocus)

			WindowFocusEvent()
			{

			}

			virtual std::string GetDebugString() const
			{
				std::stringstream ss;
				ss << "[" + GetName() + "]\n";
				return ss.str();
			}
		};
	}
}
