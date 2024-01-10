/// 2024. 01. 09 김상준
/// Event 를 사용한 void() 형태의 callable 을 관리하는 Manager 클래스

#pragma once

#include "EditorEvents.h"

#include <queue>
#include <functional>

namespace application
{
	namespace editor
	{
		class EventManager
		{
		public :
			bool PushEventCallable(std::function<void()> eventFunc);
			std::function<void()> PopEventCallable();
			size_t Size();
			void Clear();

		private:
			std::queue<std::function<void()>> eventFuncQueue;
		};
	}
}


