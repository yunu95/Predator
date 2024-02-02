/// 2024. 01. 09 김상준
/// Editor Mouse 입력 이벤트들을 위한 클래스

#pragma once

#include "EditorEvents.h"
#include "EditorKeyCodes.h"

#include <sstream>

namespace application
{
	namespace editor
	{
		class MouseButtonEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Editor | EventCategory::Input | EventCategory::Mouse)

			MouseCode GetMouseCode() const { return mouseCode; }

		protected:
			MouseButtonEvent(MouseCode mouseCode)
				: mouseCode(mouseCode)
			{
		
			}

		protected:
			MouseCode mouseCode;
		};

		class MouseButtonPressedEvent
			: public MouseButtonEvent
		{
		public:
			EVENT_SETTING(EventType::MouseButtonPressed)

			MouseButtonPressedEvent(MouseCode mouseCode)
				: MouseButtonEvent(mouseCode)
			{

			}

			virtual std::string GetDebugString() const override
			{
				std::stringstream ss;
				ss << "[" + GetName() + "] ";
				auto code = GetMouseCode();
				switch (code)
				{
					case application::editor::MouseCode::Left:
					{
						ss << "Left Pressed\n";
						break;
					}
					case application::editor::MouseCode::Middle:
					{
						ss << "Wheel Pressed\n";
						break;
					}
					case application::editor::MouseCode::Right:
					{
						ss << "Right Pressed\n";
						break;
					}
					default:
						break;
				}
				return ss.str();
			}
		};

		class MouseButtonDownEvent
			: public MouseButtonEvent
		{
		public:
			EVENT_SETTING(EventType::MouseButtonDown)

			MouseButtonDownEvent(MouseCode mouseCode)
				: MouseButtonEvent(mouseCode)
			{

			}

			virtual std::string GetDebugString() const override
			{
				std::stringstream ss;
				ss << "[" + GetName() + "] ";
				auto code = GetMouseCode();
				switch (code)
				{
					case application::editor::MouseCode::Left:
					{
						ss << "Left Down\n";
						break;
					}
					case application::editor::MouseCode::Middle:
					{
						ss << "Wheel Down\n";
						break;
					}
					case application::editor::MouseCode::Right:
					{
						ss << "Right Down\n";
						break;
					}
					default:
						break;
				}
				return ss.str();
			}
		};

		class MouseButtonUpEvent
			: public MouseButtonEvent
		{
		public:
			EVENT_SETTING(EventType::MouseButtonUp)

			MouseButtonUpEvent(MouseCode mouseCode)
				: MouseButtonEvent(mouseCode)
			{

			}

			virtual std::string GetDebugString() const override
			{
				std::stringstream ss;
				ss << "[" + GetName() + "] ";
				auto code = GetMouseCode();
				switch (code)
				{
					case application::editor::MouseCode::Left:
					{
						ss << "Left Up\n";
						break;
					}
					case application::editor::MouseCode::Middle:
					{
						ss << "Wheel Up\n";
						break;
					}
					case application::editor::MouseCode::Right:
					{
						ss << "Right Up\n";
						break;
					}
					default:
						break;
				}
				return ss.str();
			}
		};

		class MouseMoveEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Editor | EventCategory::Input | EventCategory::Mouse)
			EVENT_SETTING(EventType::MouseMove)

			MouseMoveEvent(long posX, long posY)
				: posX(posX), posY(posY)
			{

			}

			long GetPosX() { return posX; }
			long GetPosY() { return posY; }

			virtual std::string GetDebugString() const override
			{
				std::stringstream ss;
				ss << "[" + GetName() + "] posX : " << posX << " / posY : " << posY << '\n';
				return ss.str();
			}

		private:
			long posX;
			long posY;
		};

		class MouseWheelEvent
			: public EditorEvents
		{
		public:
			CATEGORY_SETTING(EventCategory::Editor | EventCategory::Input | EventCategory::Mouse)
			EVENT_SETTING(EventType::MouseWheel)

			MouseWheelEvent(short wheelDelta)
				: wheelDelta(wheelDelta)
			{

			}

			short GetWheelDelta() { return wheelDelta; }

			virtual std::string GetDebugString() const override
			{
				std::stringstream ss;
				ss << "[" + GetName() + "] wheelDelta : " << wheelDelta << '\n';
				return ss.str();
			}

		private:
			short wheelDelta;
		};
	}
}
