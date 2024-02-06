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
			CATEGORY_SETTING(EventCategory::Editor)
			EVENT_SETTING(EventType::SelectionChange)
		};

		class SaveEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Editor)
			EVENT_SETTING(EventType::Save)

			virtual std::string GetDebugString() const override
			{
				std::stringstream ss;
				ss << "[" + GetName() + "]\n";
				return ss.str();
			}
		};

		class LoadEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Editor)
			EVENT_SETTING(EventType::Load)

			virtual std::string GetDebugString() const override
			{
				std::stringstream ss;
				ss << "[" + GetName() + "]\n";
				return ss.str();
			}
		};
	}
}
