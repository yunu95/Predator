#pragma once
#include <functional>

namespace WanderUtil
{
	class Timer
	{
	public:
		Timer();
		std::function<void(float)> onTimerElapsed{ [](float) {} };
		std::function<void()> onTimerExpiration{ []() {} };

		void Start();
		void Update();

		bool isActive;
		bool isRepeat;
		float elapsed;
		float duration;
	};
}
