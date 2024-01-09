/// 2024. 01. 09 김상준
/// Editor 의 일반적인 이벤트들을 위한 클래스

#pragma once

#include "EditorEvents.h"

#include <sstream>

namespace application
{
	namespace editor
	{
		class SelectionChangeEvent
			: public EditorEvents
		{
		public:
			EVENT_SETTING(Editor, SelectionChange);
		};
	}
}
