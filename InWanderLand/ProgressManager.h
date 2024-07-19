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
        // 페이드 아웃, 페이드 인 연출을 하면서 로드를 진행합니다.
        void LoadCheckPoint();

    private:
        coroutine::Coroutine LoadCheckPointCoro();
        ProgressManager() = default;
        std::unordered_set<ProgressTracker*> trackerList = std::unordered_set<ProgressTracker*>();
    };
}

