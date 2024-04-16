/// 2024. 04. 16 김상준
/// 게임이 시작되는 순간부터 시간을 측정하는 Timer 입니다.
/// 게임의 플레이와 무관한 동작을 할 경우에, 해당 Timer 를
/// 잠깐 정지하고 다시 시작하는 등의 동작도 가능합니다.

#pragma once

#include "YunutyEngine.h"
#include "SingletonComponent.h"

#include "PlayableComponent.h"

#include <map>
#include <vector>
#include <functional>

namespace application
{
	class GamePlayTimer
		: public PlayableComponent, public Component, public SingletonComponent<GamePlayTimer>
	{
	public:
		virtual void SingletonInitializer() override;

		virtual void Update() override;

		virtual void PreMapLoad() override;
		virtual void OnGameStart() override;
		virtual void OnGamePause() override;
		virtual void OnGameResume() override;
		virtual void OnGameStop() override;

		void PauseTimer();
		void ResumeTimer();

		std::map<double, std::vector<std::function<void()>>> repeatCallback;
		std::map<double, std::vector<std::function<void()>>> repeatCallbackReal;

	private:
		bool shouldCheck = false;
		double playTime = 0;
		double playRealTime = 0;

		std::map<double, double> repeatTimeChecker;
		std::map<double, double> repeatTimeCheckerReal;
	};
}

