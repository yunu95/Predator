/// 2024. 01. 09 김상준
/// 에디터에서 사용할 Event 에 대한 추상 클래스
/// Command 와 달리, 유저의 행동에 기인한 단위 행동이라기보다
/// 발현된 이벤트가 전파되며 자신의 부분에서 해당 이벤트에 대하여 처리할 수 있는
/// 구조를 사용하여 각 부분에서 해당 구현을 할 수 있도록 마련함

#pragma once

#ifdef EDITOR
#include <string>
#include <functional>

#include <iostream>

namespace application
{
	namespace editor
	{
		// Flag
		enum class EventCategory : unsigned long long
		{
			None = 0,
			// Common Event
			Editor = 1,
			Input = 1 << 1,
			Window = 1 << 2,
			InGame = 1 << 3,
			Mouse = 1 << 4,
			Keyboard = 1 << 5,
		};

#pragma region operator overloading
		unsigned long long operator | (const EventCategory& c1, const EventCategory& c2);
		unsigned long long operator | (const unsigned long long& c1, const EventCategory& c2);
		unsigned long long operator | (const EventCategory& c1, const unsigned long long&& c2);
		unsigned long long operator & (const EventCategory& c1, const EventCategory& c2);
		unsigned long long operator & (const unsigned long long& c1, const EventCategory& c2);
		unsigned long long operator & (const EventCategory& c1, const unsigned long long& c2);
#pragma endregion

		enum class EventType
		{
			None = 0,
			// Editor
			SelectionChange,
			// Window
			WindowResize, WindowMove, WindowFocus,
			// Mouse
			MouseButtonPressed, MouseButtonDown, MouseButtonUp, MouseMove, MouseWheel,
			// Keyboard
			KeyPressed, KeyDown, KeyReleased,
		};

		// 멤버 함수 override 자동화를 위한 매크로 함수
		#define CATEGORY_SETTING(category) \
		static unsigned long long GetStaticCategoryFlag() { return static_cast<unsigned long long>(category); } \
		virtual unsigned long long GetEventCategoryFlag() const override { return GetStaticCategoryFlag(); }

		#define EVENT_SETTING(type) \
		static EventType GetStaticType() { return type; } \
		virtual EventType GetEventType() const override { return GetStaticType(); } \
		virtual std::string GetName() const override { return #type; }


		class EditorEvents
		{
		public:
			virtual ~EditorEvents() = default;
			virtual unsigned long long GetEventCategoryFlag() const = 0;
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
				if (event.propagation && (event.GetEventCategoryFlag() & T::GetStaticCategoryFlag()) && (event.GetEventType() == T::GetStaticType()))
				{
					event.propagation = func(*static_cast<T*>(&event));
					return true;
				}
				else
				{
					/// 디버그 용
					if (!event.propagation)
					{
						std::cout << event.GetName() << " 전파 종료 이벤트입니다." << std::endl;
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
#endif
