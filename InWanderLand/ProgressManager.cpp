#include "ProgressManager.h"
#include "InWanderLand.h"

#include "ProgressTracker.h"

namespace application
{
    void ProgressManager::NotifyInitializeAll()
    {
        for (auto& each : trackerList)
        {
            each->ProgressInitialize();
        }
    }

    void ProgressManager::NotifyCurrentProgressSaveAll()
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::CheckPointReached)->EnableElement();
        for (auto& each : trackerList)
        {
            each->CurrentProgressSave();
        }
    }

    void ProgressManager::NotifyRecoveryAll()
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::DefeatPage)->DisableElement();
        for (auto& each : trackerList)
        {
            each->Recovery();
        }
    }

    void ProgressManager::OnGameStart()
    {
        NotifyInitializeAll();
    }
    void ProgressManager::LoadCheckPoint()
    {
        ContentsCoroutine::Instance().StartCoroutine(LoadCheckPointCoro());
    }
    coroutine::Coroutine ProgressManager::LoadCheckPointCoro()
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_Alpha)->EnableElement();
        coroutine::ForSeconds forSeconds{ 1.1f, true };
        while (forSeconds.Tick()) { co_await std::suspend_always{}; }
        NotifyRecoveryAll();
        coroutine::ForSeconds forSecondsAfter{ 0.5f, true };
        while (forSecondsAfter.Tick()) { co_await std::suspend_always{}; }
        UIManager::Instance().HideComboObjectvies();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_Alpha)->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::InGameMenu)->DisableElement();

        co_return;
    }
}
