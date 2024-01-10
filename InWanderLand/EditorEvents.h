/// 2024. 01. 09 김상준
/// 에디터에서 사용할 Event 에 대한 추상 클래스
/// Command 와 달리, 유저의 행동에 기인한 단위 행동이라기보다
/// 발현된 이벤트가 전파되며 자신의 부분에서 해당 이벤트에 대하여 처리할 수 있는
/// 구조를 사용하여 각 부분에서 해당 구현을 할 수 있도록 마련함

#pragma once

#include <string>
#include <functional>

#include <iostream>

namespace application
{
	namespace editor
	{
		enum class EventCategory
		{
			None = 0,
			// Common Event
			Editor,
			Window,
			Mouse,
			Keyboard,
		};

		enum class EventType
		{
			None = 0,
			// Editor
			SelectionChange,
			// Window
			WindowResize, WindowMove, WindowFocus,
			// Mouse
			MouseButtonPressed, MouseButtonReleased, MouseButtonDown, MouseMoved, MouseScrolled,
			// Keyboard
			KeyPressed, KeyReleased,
		};

		// 멤버 함수 override 자동화를 위한 매크로 함수
		#define EVENT_SETTING(category, type) \
		static EventCategory GetStaticCategory() { return EventCategory::##category; } \
		static EventType GetStaticType() { return EventType::##type; } \
		virtual EventCategory GetEventCategory() const override { return GetStaticCategory(); } \
		virtual EventType GetEventType() const override { return GetStaticType(); } \
		virtual std::string GetName() const override { return #type; }

		class EditorEvents
		{
		public:
			virtual ~EditorEvents() = default;
			virtual EventCategory GetEventCategory() const = 0;
			virtual EventType GetEventType() const = 0;
			// 디버그 용도로 쓰기 위함
			virtual std::string GetName() const = 0;
			virtual std::string GetDebugString() const { return std::string(); }

		public:
			bool propagation = true;
		};

		// EditorEvents 로부터 생성하여 해당 Event에 대한 callable 을 호출해주는 클래스
		class EventDispatcher
		{
			template<typename T>
			using EventFunc = std::function<bool(T&)>;

		public:
			EventDispatcher(EditorEvents& event)
				: event(event)
			{

			}

			template<typename T>
			bool Dispatch(EventFunc<T> func)
			{
				if (event.propagation && (event.GetEventCategory() == T::GetStaticCategory()) && (event.GetEventType() == T::GetStaticType()))
				{
					event.propagation = func(*static_cast<T*>(&event));
					return true;
				}
				else
				{
					/// 디버그 용
					if (!event.propagation)
					{
						std::cout << event.GetName() << " 이벤트 전파 종료!" << std::endl;
					}
					///

					return false;
				}
			}

		private:
			EditorEvents& event;
		};
	}
}
