/// 2024. 07. 17 김상준
/// 게임 진행 상황에 대해 기록하는 class 입니다.

#pragma once
#include "YunutyEngine.h"
#include "PlayableComponent.h"

namespace application
{
	class ProgressTracker;
	class ProgressManager
		: public SingletonClass<ProgressManager>, public application::PlayableComponent
	{
		friend class SingletonClass<ProgressManager>;
		friend class ProgressTracker;

	public:
		void NotifyInitializeAll();
		void NotifyCurrentProgressSaveAll();
		void NotifyRecoveryAll();

		virtual void OnGameStart();

	private:
		ProgressManager() = default;
		std::unordered_set<ProgressTracker*> trackerList = std::unordered_set<ProgressTracker*>();
	};
}

