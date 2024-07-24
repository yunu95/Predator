#include "Script.h"

#include "algorithm"

namespace application
{
    void Script::Update()
    {
        auto queueSize = coroutineQueue.size();
        for (int i = 0; i < queueSize; i++)
        {
            auto coroutine = coroutineQueue.front();
            if (coroutine.Done())
            {
                coroutineQueue.pop();
                continue;
            }
            coroutine();
            coroutineQueue.pop();
            coroutineQueue.push(coroutine);
        }
    }

    void Script::OnGameStart()
    {
        for (auto& each : triggerList)
        {
            each->Clear();
            each->AddFunction([=]()
                {
                    Script::PullScriptTrigger();
                });
            each->LinkCallback();
        }
    }

    void Script::OnGameStop()
    {
        coroutineInProgress.clear();
        auto size = coroutineQueue.size();
        for (int i = 0; i < size; i++)
        {
            coroutineQueue.pop();
        }
    }

    bool Script::EraseTrigger(const std::shared_ptr<ITrigger>& ptr)
    {
        if (triggerList.find(ptr) == triggerList.end())
        {
            return false;
        }

        triggerList.erase(ptr);

        return true;
    }

    bool Script::EraseCondition(const std::shared_ptr<ICondition>& ptr)
    {
        if (conditionList.find(ptr) == conditionList.end())
        {
            return false;
        }

        conditionList.erase(ptr);

        return true;
    }

    bool Script::EraseAction(const std::shared_ptr<IAction>& ptr)
    {
        auto itr = std::find(actionList.begin(), actionList.end(), ptr);
        if (itr == actionList.end())
        {
            return false;
        }

        actionList.erase(itr);

        return true;
    }

    bool Script::ReorderAction(const std::shared_ptr<IAction>& ptr, unsigned int idx)
    {
        int index = 0;
        for (auto& each : actionList)
        {
            if (each == ptr)
            {
                if (index == idx)
                {
                    return false;
                }

                auto swapPtr = actionList[idx];
                actionList[index] = swapPtr;
                actionList[idx] = ptr;

                return true;
            }
            index++;
        }

        return false;
    }

    void Script::ProgressInitialize()
    {
        doAction = false;
        savedAction = false;
    }

    void Script::CurrentProgressSave()
    {
        savedAction = doAction;
    }

    void Script::Recovery()
    {
        doAction = savedAction;
    }

    void Script::PullScriptTrigger()
    {
        if (!repeat && doAction)
        {
            return;
        }

        bool isConditionMet = true;
        for (auto& each : conditionList)
        {
            isConditionMet &= each->IsConditionMet();
        }

        if (isConditionMet)
        {
            PushCoroutine();
            doAction = true;
        }
    }

    bool Script::PreEncoding(json& data) const
    {
        std::string uuidStr;
        data["name"] = name;
        data["repeat"] = repeat;

        for (auto& each : triggerList)
        {
            uuidStr = UUID_To_String(each->GetUUID());
            data["TriggerList"][uuidStr]["type"] = each->GetType();
            if (!each->PreEncoding(data["TriggerList"][uuidStr]["0_Pre"]))
            {
                return false;
            }
        }

        for (auto& each : conditionList)
        {
            uuidStr = UUID_To_String(each->GetUUID());
            data["ConditionList"][uuidStr]["type"] = each->GetType();
            if (!each->PreEncoding(data["ConditionList"][uuidStr]["0_Pre"]))
            {
                return false;
            }
        }

        int index = 0;
        for (auto& each : actionList)
        {
            uuidStr = UUID_To_String(each->GetUUID());
            data["ActionList"][index][uuidStr]["type"] = each->GetType();
            data["ActionList"][index][uuidStr]["repeat"] = each->repeat;
            if (!each->PreEncoding(data["ActionList"][index][uuidStr]["0_Pre"]))
            {
                return false;
            }
            index++;
        }

        return true;
    }

    bool Script::PostEncoding(json& data) const
    {
        std::string uuidStr;
        for (auto& each : triggerList)
        {
            uuidStr = UUID_To_String(each->GetUUID());

            auto itr = data["TriggerList"].find(uuidStr);
            if (itr == data["TriggerList"].end())
            {
                return false;
            }

            if (!each->PostEncoding(itr.value()["1_Post"]))
            {
                return false;
            }
        }

        for (auto& each : conditionList)
        {
            uuidStr = UUID_To_String(each->GetUUID());

            auto itr = data["ConditionList"].find(uuidStr);
            if (itr == data["ConditionList"].end())
            {
                return false;
            }

            if (!each->PostEncoding(itr.value()["1_Post"]))
            {
                return false;
            }
        }

        int index = 0;
        for (auto& each : actionList)
        {
            uuidStr = UUID_To_String(each->GetUUID());

            auto itr = data["ActionList"][index].find(uuidStr);
            if (itr == data["ActionList"][index].end())
            {
                return false;
            }

            if (!each->PostEncoding(itr.value()["1_Post"]))
            {
                return false;
            }
            index++;
        }

        return true;
    }

    bool Script::PreDecoding(const json& data)
    {
        UUID uuid;
        name = data["name"];

        if (data.contains("repeat"))
        {
            repeat = data["repeat"];
        }

        if (data.contains("TriggerList"))
        {
            for (auto& [uuidStr, triggerData] : data["TriggerList"].items())
            {
                uuid = String_To_UUID(uuidStr);

                TriggerType type = triggerData["type"];

                std::shared_ptr<ITrigger> trigger = nullptr;

                switch (type)
                {
                case application::TriggerType::GameStart:
                {
                    trigger = AddTrigger<Trigger_GameStart>();
                    break;
                }
                case application::TriggerType::EnterRegion:
                {
                    trigger = AddTrigger<Trigger_EnterRegion>();
                    break;
                }
                case application::TriggerType::LeaveRegion:
                {
                    trigger = AddTrigger<Trigger_LeaveRegion>();
                    break;
                }
                case application::TriggerType::RepeatPeriodically:
                {
                    trigger = AddTrigger<Trigger_RepeatPeriodically>();
                    break;
                }
                case application::TriggerType::RepeatPeriodicallyRealTime:
                {
                    trigger = AddTrigger<Trigger_RepeatPeriodicallyRealTime>();
                    break;
                }
                case application::TriggerType::UnitAppear:
                {
                    trigger = AddTrigger<Trigger_UnitAppear>();
                    break;
                }
                case application::TriggerType::UnitDie:
                {
                    trigger = AddTrigger<Trigger_UnitDie>();
                    break;
                }
                case application::TriggerType::InteractableTriggerEnter:
                {
                    trigger = AddTrigger<Trigger_InteractableTriggerEnter>();
                    break;
                }
                case application::TriggerType::InteractableTriggerExit:
                {
                    trigger = AddTrigger<Trigger_InteractableTriggerExit>();
                    break;
                }
                case application::TriggerType::StartWave:
                {
                    trigger = AddTrigger<Trigger_StartWave>();
                    break;
                }
                case application::TriggerType::EndWave:
                {
                    trigger = AddTrigger<Trigger_EndWave>();
                    break;
                }
                case application::TriggerType::StageStart:
                {
                    trigger = AddTrigger<Trigger_StageStart>();
                    break;
                }
                case application::TriggerType::Shortcut:
                {
                    trigger = AddTrigger<Trigger_Shortcut>();
                    break;
                }
                default:
                    break;
                }

                if (trigger == nullptr)
                {
                    return false;
                }

                // UUID
                trigger->SetUUID(uuid);

                if (!trigger->PreDecoding(triggerData["0_Pre"]))
                {
                    return false;
                }
            }
        }

        if (data.contains("ConditionList"))
        {
            for (auto& [uuidStr, conditionData] : data["ConditionList"].items())
            {
                uuid = String_To_UUID(uuidStr);

                ConditionType type = conditionData["type"];

                std::shared_ptr<ICondition> condition = nullptr;

                switch (type)
                {
                case application::ConditionType::CinematicModeOn:
                {
                    condition = AddCondition<Condition_CinematicModeOn>();
                    break;
                }
                case application::ConditionType::CinematicModeOff:
                {
                    condition = AddCondition<Condition_CinematicModeOff>();
                    break;
                }
                case application::ConditionType::TutorialModeOn:
                {
                    condition = AddCondition<Condition_TutorialModeOn>();
                    break;
                }
                case application::ConditionType::TutorialModeOff:
                {
                    condition = AddCondition<Condition_TutorialModeOff>();
                    break;
                }
                case application::ConditionType::TacticModeOn:
                {
                    condition = AddCondition<Condition_TacticModeOn>();
                    break;
                }
                case application::ConditionType::TacticModeOff:
                {
                    condition = AddCondition<Condition_TacticModeOff>();
                    break;
                }
                case application::ConditionType::PlayerState:
                {
                    condition = AddCondition<Condition_PlayerState>();
                    break;
                }
                case application::ConditionType::OrnamentShow:
                {
                    condition = AddCondition<Condition_OrnamentShow>();
                    break;
                }
                case application::ConditionType::OrnamentHide:
                {
                    condition = AddCondition<Condition_OrnamentHide>();
                    break;
                }
                case application::ConditionType::Stage:
                {
                    condition = AddCondition<Condition_Stage>();
                    break;
                }
                default:
                    break;
                }

                if (condition == nullptr)
                {
                    return false;
                }

                // UUID
                condition->SetUUID(uuid);

                if (!condition->PreDecoding(conditionData["0_Pre"]))
                {
                    return false;
                }
            }
        }

        if (data.contains("ActionList"))
        {
            for (auto& [idx, uuidData] : data["ActionList"].items())
            {
                for (auto& [uuidStr, actionData] : uuidData.items())
                {
                    uuid = String_To_UUID(uuidStr);

                    ActionType type = actionData["type"];

                    std::shared_ptr<IAction> action = nullptr;

                    switch (type)
                    {
                    case application::ActionType::WaitForSeconds:
                    {
                        action = AddAction<Action_WaitForSeconds>();
                        break;
                    }
                    case application::ActionType::WaitForRealSeconds:
                    {
                        action = AddAction<Action_WaitForRealSeconds>();
                        break;
                    }
                    case application::ActionType::CinematicModeChange:
                    {
                        action = AddAction<Action_CinematicModeChange>();
                        break;
                    }
                    case application::ActionType::CameraChangeView:
                    {
                        action = AddAction<Action_CameraChangeView>();
                        break;
                    }
                    case application::ActionType::CameraSaveView:
                    {
                        action = AddAction<Action_CameraSaveView>();
                        break;
                    }
                    case application::ActionType::CameraLoadView:
                    {
                        action = AddAction<Action_CameraLoadView>();
                        break;
                    }
                    case application::ActionType::CinematicFadeIn:
                    {
                        action = AddAction<Action_CinematicFadeIn>();
                        break;
                    }
                    case application::ActionType::CinematicFadeOut:
                    {
                        action = AddAction<Action_CinematicFadeOut>();
                        break;
                    }
                    case application::ActionType::WaitPreviousActionEnd:
                    {
                        action = AddAction<Action_WaitPreviousActionEnd>();
                        break;
                    }
                    case application::ActionType::CameraRevert:
                    {
                        action = AddAction<Action_CameraRevert>();
                        break;
                    }
                    case application::ActionType::TutorialModeChange:
                    {
                        action = AddAction<Action_TutorialModeChange>();
                        break;
                    }
                    case application::ActionType::PlayManualDialogue:
                    {
                        action = AddAction<Action_PlayManualDialogue>();
                        break;
                    }
                    case application::ActionType::PlayTimedDialogue:
                    {
                        action = AddAction<Action_PlayTimedDialogue>();
                        break;
                    }
                    case application::ActionType::UnitMove:
                    {
                        action = AddAction<Action_UnitMove>();
                        break;
                    }
                    case application::ActionType::UnitRotate:
                    {
                        action = AddAction<Action_UnitRotate>();
                        break;
                    }
                    case application::ActionType::UnitRescale:
                    {
                        action = AddAction<Action_UnitRescale>();
                        break;
                    }
                    case application::ActionType::UnitMoveWithRotateAndRescale:
                    {
                        action = AddAction<Action_UnitMoveWithRotateAndRescale>();
                        break;
                    }
                    case application::ActionType::UnitRelocate:
                    {
                        action = AddAction<Action_UnitRelocate>();
                        break;
                    }
                    case application::ActionType::UnitPauseAll:
                    {
                        action = AddAction<Action_UnitPauseAll>();
                        break;
                    }
                    case application::ActionType::PlayerSelect:
                    {
                        action = AddAction<Action_PlayerSelect>();
                        break;
                    }
                    case application::ActionType::BlockPlayerSwitch:
                    {
                        action = AddAction<Action_BlockPlayerSwitch>();
                        break;
                    }
                    case application::ActionType::BlockSkillCancel:
                    {
                        action = AddAction<Action_BlockSkillCancel>();
                        break;
                    }
                    case application::ActionType::BlockSkillSelection:
                    {
                        action = AddAction<Action_BlockSkillSelection>();
                        break;
                    }
                    case application::ActionType::AwaitSkillSelection:
                    {
                        action = AddAction<Action_AwaitSkillSelection>();
                        break;
                    }
                    case application::ActionType::AwaitSkillActivation:
                    {
                        action = AddAction<Action_AwaitSkillActivation>();
                        break;
                    }
                    case application::ActionType::SetTimeScale:
                    {
                        action = AddAction<Action_SetTimeScale>();
                        break;
                    }
                    case application::ActionType::PauseWaves:
                    {
                        action = AddAction<Action_PauseWaves>();
                        break;
                    }
                    case application::ActionType::EngageStage:
                    {
                        action = AddAction<Action_EngageStage>();
                        break;
                    }
                    case application::ActionType::SoundPlay:
                    {
                        action = AddAction<Action_SoundPlay>();
                        break;
                    }
                    case application::ActionType::SoundPlayMusic:
                    {
                        action = AddAction<Action_SoundPlayMusic>();
                        break;
                    }
                    case application::ActionType::SoundSetMusicVolume:
                    {
                        action = AddAction<Action_SoundSetMusicVolume>();
                        break;
                    }
                    case application::ActionType::UISetActive:
                    {
                        action = AddAction<Action_UISetActive>();
                        break;
                    }
                    case application::ActionType::OrnamentMoveWithRotateAndRescale:
                    {
                        action = AddAction<Action_OrnamentMoveWithRotateAndRescale>();
                        break;
                    }
                    case application::ActionType::OrnamentShow:
                    {
                        action = AddAction<Action_OrnamentShow>();
                        break;
                    }
                    case application::ActionType::OrnamentHide:
                    {
                        action = AddAction<Action_OrnamentHide>();
                        break;
                    }
                    case application::ActionType::OrnamentFloating:
                    {
                        action = AddAction<Action_OrnamentFloating>();
                        break;
                    }
                    case application::ActionType::BossAppear:
                    {
                        action = AddAction<Action_BossAppear>();
                        break;
                    }
                    case application::ActionType::RequestState:
                    {
                        action = AddAction<Action_RequestState>();
                        break;
                    }
                    case application::ActionType::AwaitSkillTargeting:
                    {
                        action = AddAction<Action_AwaitSkillTargeting>();
                        break;
                    }
                    case application::ActionType::AwaitSkillExpiration:
                    {
                        action = AddAction<Action_AwaitSkillExpiration>();
                        break;
                    }
                    case application::ActionType::ForceUnitPaused:
                    {
                        action = AddAction<Action_ForceUnitPaused>();
                        break;
                    }
                    case application::ActionType::ForceUnitUnpaused:
                    {
                        action = AddAction<Action_ForceUnitUnpaused>();
                        break;
                    }
                    case application::ActionType::SetZoomFactor:
                    {
                        action = AddAction<Action_SetZoomFactor>();
                        break;
                    }
                    case application::ActionType::SetCamBidirectionalPath:
                    {
                        action = AddAction<Action_SetCamBidirectionalPath>();
                        break;
                    }
                    case application::ActionType::ParticleMoveWithRotateAndRescale:
                    {
                        action = AddAction<Action_ParticleMoveWithRotateAndRescale>();
                        break;
                    }
                    case application::ActionType::ParticleShow:
                    {
                        action = AddAction<Action_ParticleShow>();
                        break;
                    }
                    case application::ActionType::ParticleHide:
                    {
                        action = AddAction<Action_ParticleHide>();
                        break;
                    }
                    case application::ActionType::UnitPlayAnimation:
                    {
                        action = AddAction<Action_UnitPlayAnimation>();
                        break;
                    }
                    case application::ActionType::PullScriptTrigger:
                    {
                        action = AddAction<Action_PullScriptTrigger>();
                        break;
                    }
                    case application::ActionType::SetTacticCamera:
                    {
                        action = AddAction<Action_SetTacticCamera>();
                        break;
                    }
                    case application::ActionType::SetRegionAsNavObstacle:
                    {
                        action = AddAction<Action_SetRegionAsNavObstacle>();
                        break;
                    }
                    case application::ActionType::SetPlayerConstrainingRegion:
                    {
                        action = AddAction<Action_SetPlayerConstrainingRegion>();
                        break;
                    }
                    case application::ActionType::ProgressSave:
                    {
                        action = AddAction<Action_ProgressSave>();
                        break;
                    }
                    case application::ActionType::ProgressLoad:
                    {
                        action = AddAction<Action_ProgressLoad>();
                        break;
                    }
                    case application::ActionType::UnitSetHP:
                    {
                        action = AddAction<Action_UnitSetHP>();
                        break;
                    }
                    default:
                        break;
                    }

                    if (action == nullptr)
                    {
                        return false;
                    }

                    // UUID
                    action->SetUUID(uuid);

                    if (actionData.contains("repeat"))
                    {
                        action->repeat = actionData["repeat"];
                    }
                    if (!action->PreDecoding(actionData["0_Pre"]))
                    {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    bool Script::PostDecoding(const json& data)
    {
        UUID uuid;
        if (data.contains("TriggerList"))
        {
            for (auto& [uuidStr, triggerData] : data["TriggerList"].items())
            {
                uuid = String_To_UUID(uuidStr);

                for (auto& each : triggerList)
                {
                    if (each->id == uuid)
                    {
                        if (!each->PostDecoding(triggerData["1_Post"]))
                        {
                            return false;
                        }
                        break;
                    }
                }
            }
        }

        if (data.contains("ConditionList"))
        {
            for (auto& [uuidStr, conditionData] : data["ConditionList"].items())
            {
                uuid = String_To_UUID(uuidStr);

                for (auto& each : conditionList)
                {
                    if (each->id == uuid)
                    {
                        if (!each->PostDecoding(conditionData["1_Post"]))
                        {
                            return false;
                        }
                        break;
                    }
                }
            }
        }

        if (data.contains("ActionList"))
        {
            for (auto& [idx, uuidData] : data["ActionList"].items())
            {
                for (auto& [uuidStr, actionData] : uuidData.items())
                {
                    uuid = String_To_UUID(uuidStr);

                    for (auto& each : actionList)
                    {
                        if (each->id == uuid)
                        {
                            if (!each->PostDecoding(actionData["1_Post"]))
                            {
                                return false;
                            }
                            break;
                        }
                    }
                }
            }
        }

        return true;
    }

    void Script::PushCoroutine()
    {
        coroutineQueue.push(MakeActionCoroutine());
    }

    CoroutineObject<void> Script::MakeActionCoroutine()
    {
        for (auto& action : actionList)
        {
            if (!action->repeat && action->doAction)
            {
                continue;
            }

            action->doAction = true;
            auto coroutine = action->DoAction();
            coroutineInProgress.emplace_back(coroutine);
            if (action->GetType() == ActionType::WaitForSeconds
                || action->GetType() == ActionType::WaitForRealSeconds)
            {
                while (!coroutine.Done())
                {
                    coroutine();
                    co_await std::suspend_always();
                }
            }
            else if (action->GetType() == ActionType::WaitPreviousActionEnd)
            {
                if (coroutineInProgress.size() >= 2)
                {
                    while (!coroutineInProgress[coroutineInProgress.size() - 2].Done())
                    {
                        co_await std::suspend_always();
                    }
                }
                coroutine();
            }
            else
            {
                coroutineQueue.push(coroutine);
            }
        }
    }
}
