//#pragma once
//class EditorEvent
//{
//};
//
//class ClickEvent : EditorEvent
//{
//
//};
//class ButtonEvent : EditorEvent
//{
//
//};
//
//
//#include <memory>
//#include <concepts>
//#include <functional>
//
//namespace application
//{
//	namespace Editor
//	{
//		template<typename ChildPanelClass>
//		class Panel
//		{
//		public:
//			virtual void Initialize() = 0;
//			//virtual void EventProgress(Events& e) = 0;
//			virtual void Update(float ts) = 0;
//			virtual void GUIProgress() = 0;
//			virtual void Finalize() = 0;
//
//			// inside EventSystem.h
//			std::vector<std::function<void()>> eventQueue;
//
//			template<typename EventType>
//			void ReserveEvent(EventType occuredEvent)
//			{
//				eventQueue.push_back([=]() {this->FireEvent<EventType>(occuredEvent); });
//			}
//			void Progress()
//			{
//				for (auto each : eventQueue)
//					each();
//			}
//
//			template<typename EventType>
//			void FireEvent(EventType occuredEvent)
//			{
//				for (each : panels)
//					each.OnEvent(occuredEvent);
//			};
//
//			template<typename EventType>
//			void OnEvent(EventType occuredEvent)
//			{
//				if constexpr ( /*ChildPanelClass에게 OnEvent함수가 있다면 이것을 실행한다.*/)
//					reinterpret_cast<ChildPanelClass*>(this)->OnEvent(occuredEvent);
//			}
//
//			bool IsPanelMouseOver() { return isMouseOver; }
//			bool IsPanelFocused() { return isFocused; }
//
//
//		protected:
//			bool isMouseOver = false;
//			bool isFocused = false;
//		};
//	}
//}
//
