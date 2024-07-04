#include "ScriptSystem.h"

#include "Script.h"
#include "YunutyEngine.h"

namespace application
{
    std::vector<std::pair<TriggerType, std::string>> ScriptSystem::triggerList = std::vector<std::pair<TriggerType, std::string>>();
    std::vector<std::pair<ConditionType, std::string>> ScriptSystem::conditionList = std::vector<std::pair<ConditionType, std::string>>();
    std::vector<std::pair<ActionType, std::string>> ScriptSystem::actionList = std::vector<std::pair<ActionType, std::string>>();

    void ScriptSystem::SingletonInitializer()
    {
        /// Trigger
        triggerList.push_back({ TriggerType::GameStart,"GameStart" });
        triggerList.push_back({ TriggerType::EnterRegion, "EnterRegion" });
        triggerList.push_back({ TriggerType::LeaveRegion, "LeaveRegion" });
        triggerList.push_back({ TriggerType::RepeatPeriodically, "RepeatPeriodically" });
        triggerList.push_back({ TriggerType::RepeatPeriodicallyRealTime, "RepeatPeriodicallyRealTime" });
        triggerList.push_back({ TriggerType::UnitAppear, "UnitAppear" });
        triggerList.push_back({ TriggerType::UnitDie, "UnitDie" });
        triggerList.push_back({ TriggerType::InteractableTriggerEnter, "InteractableTriggerEnter" });
        triggerList.push_back({ TriggerType::InteractableTriggerExit, "InteractableTriggerExit" });
        triggerList.push_back({ TriggerType::StartWave, "StartWave" });
        triggerList.push_back({ TriggerType::EndWave, "EndWave" });
        triggerList.push_back({ TriggerType::StageStart, "StageStart" });

        /// Condition
        conditionList.push_back({ ConditionType::CinematicModeOn, "CinematicModeOn" });
        conditionList.push_back({ ConditionType::CinematicModeOff, "CinematicModeOff" });
        conditionList.push_back({ ConditionType::TutorialModeOn, "TutorialModeOn" });
        conditionList.push_back({ ConditionType::TutorialModeOff, "TutorialModeOff" });
        conditionList.push_back({ ConditionType::TacticModeOn, "TacticModeOn" });
        conditionList.push_back({ ConditionType::TacticModeOff, "TacticModeOff" });
        conditionList.push_back({ ConditionType::PlayerState, "PlayerState" });
        conditionList.push_back({ ConditionType::OrnamentShow, "OrnamentShow" });
        conditionList.push_back({ ConditionType::OrnamentHide, "OrnamentHide" });
        conditionList.push_back({ ConditionType::Stage, "Stage" });

        /// Action
        actionList.push_back({ ActionType::WaitPreviousActionEnd, "WaitPreviousActionEnd" });
        actionList.push_back({ ActionType::WaitForSeconds, "WaitForSeconds" });
        actionList.push_back({ ActionType::WaitForRealSeconds, "WaitForRealSeconds" });
        actionList.push_back({ ActionType::CinematicModeChange, "CinematicModeChange" });
        actionList.push_back({ ActionType::CinematicFadeIn, "CinematicFadeIn" });
        actionList.push_back({ ActionType::CinematicFadeOut, "CinematicFadeOut" });
        actionList.push_back({ ActionType::CameraChangeView, "CameraChangeView" });
        actionList.push_back({ ActionType::CameraRevert, "CameraRevert" });
        actionList.push_back({ ActionType::CameraSaveView, "CameraSaveView" });
        actionList.push_back({ ActionType::CameraLoadView, "CameraLoadView" });
        actionList.push_back({ ActionType::TutorialModeChange, "TutorialModeChange" });
        actionList.push_back({ ActionType::PlayManualDialogue, "PlayManualDialogue" });
        actionList.push_back({ ActionType::PlayTimedDialogue, "PlayTimedDialogue" });
        actionList.push_back({ ActionType::UnitMoveWithRotateAndRescale, "UnitMoveWithRotateAndRescale" });
        actionList.push_back({ ActionType::UnitMove, "UnitMove" });
        actionList.push_back({ ActionType::UnitRotate, "UnitRotate" });
        actionList.push_back({ ActionType::UnitRescale, "UnitRescale" });
        actionList.push_back({ ActionType::UnitRelocate,"UnitRelocate" });
        actionList.push_back({ ActionType::UnitPauseAll,"UnitPauseAll" });
        actionList.push_back({ ActionType::PlayerSelect,"PlayerSelect" });
        actionList.push_back({ ActionType::BlockPlayerSwitch,"BlockPlayerSwitch" });
        actionList.push_back({ ActionType::BlockSkillCancel,"BlockSkillCancel" });
        actionList.push_back({ ActionType::BlockSkillSelection,"BlockSkillSelection" });
        actionList.push_back({ ActionType::AwaitSkillSelection,"AwaitSkillSelection" });
        actionList.push_back({ ActionType::AwaitSkillActivation,"AwaitSkillActivation" });
        actionList.push_back({ ActionType::SetTimeScale,"SetTimeScale" });
        actionList.push_back({ ActionType::PauseWaves,"PauseWaves" });
        actionList.push_back({ ActionType::EngageStage,"EngageStage" });
        actionList.push_back({ ActionType::SoundPlay,"SoundPlay" });
        actionList.push_back({ ActionType::SoundPlayMusic,"SoundPlayMusic" });
        actionList.push_back({ ActionType::SoundSetMusicVolume,"SoundSetMusicVolume" });
        actionList.push_back({ ActionType::UISetActive,"UISetActive" });
        actionList.push_back({ ActionType::OrnamentMoveWithRotateAndRescale,"OrnamentMoveWithRotateAndRescale" });
        actionList.push_back({ ActionType::OrnamentShow,"OrnamentShow" });
        actionList.push_back({ ActionType::OrnamentHide,"OrnamentHide" });
        actionList.push_back({ ActionType::OrnamentFloating,"OrnamentFloating" });
        actionList.push_back({ ActionType::BossAppear,"BossAppear" });
        actionList.push_back({ ActionType::RequestState,"RequestState" });
        actionList.push_back({ ActionType::AwaitSkillTargeting,"AwaitSkillTargeting" });
        actionList.push_back({ ActionType::AwaitSkillExpiration,"AwaitSkillExpiration" });
        actionList.push_back({ ActionType::ForceUnitPaused,"ForceUnitPaused" });
        actionList.push_back({ ActionType::ForceUnitUnpaused,"ForceUnitUnpaused" });
        actionList.push_back({ ActionType::SetZoomFactor,"SetZoomFactor" });
        actionList.push_back({ ActionType::SetCamBidirectionalPath,"SetCamBidirectionalPath" });
        actionList.push_back({ ActionType::ParticleMoveWithRotateAndRescale,"ParticleMoveWithRotateAndRescale" });
        actionList.push_back({ ActionType::ParticleShow,"ParticleShow" });
        actionList.push_back({ ActionType::ParticleHide,"ParticleHide" });
        actionList.push_back({ ActionType::UnitPlayAnimation,"UnitPlayAnimation" });
        actionList.push_back({ ActionType::PullScriptTrigger,"PullScriptTrigger" });

        std::sort(triggerList.begin(), triggerList.end(), [](const std::pair<TriggerType, std::string>& a, const std::pair<TriggerType, std::string>& b) { return a.second < b.second; });
        std::sort(conditionList.begin(), conditionList.end(), [](const std::pair<ConditionType, std::string>& a, const std::pair<ConditionType, std::string>& b) { return a.second < b.second; });
        std::sort(actionList.begin(), actionList.end(), [](const std::pair<ActionType, std::string>& a, const std::pair<ActionType, std::string>& b) { return a.second < b.second; });
    }

    Script* ScriptSystem::CreateScript()
    {
        auto sptr = Scene::getCurrentScene()->AddGameObject()->AddComponent<Script>();
        scriptList[sptr->GetUUID()] = sptr;
        scriptContainer.insert(sptr);
        return sptr;
    }

    bool ScriptSystem::EraseScript(Script* script)
    {
        auto itr = scriptList.find(script->GetUUID());
        if (itr == scriptList.end())
        {
            return false;
        }

        Scene::getCurrentScene()->DestroyGameObject(script->GetGameObject());

        scriptList.erase(itr);
        scriptContainer.erase(script);

        return true;
    }

    void ScriptSystem::PreMapLoad()
    {
        Action_CameraSaveView::Clear();

        Clear();
    }

    void ScriptSystem::OnGameStart()
    {
        Action_CameraSaveView::Clear();

        for (auto& each : scriptList)
        {
            each.second->OnGameStart();
        }
    }

    void ScriptSystem::OnGameStop()
    {
        Action_CameraSaveView::Clear();

        for (auto& each : scriptList)
        {
            each.second->OnGameStop();
        }
    }

    std::unordered_set<Script*>& ScriptSystem::GetScriptList()
    {
        return scriptContainer;
    }

    void ScriptSystem::Clear()
    {
        for (auto& each : scriptList)
        {
            Scene::getCurrentScene()->DestroyGameObject(each.second->GetGameObject());
        }
        scriptList.clear();
        scriptContainer.clear();
    }

    bool ScriptSystem::PreEncoding(json& data) const
    {
        std::string uuidStr;
        for (auto& [uuid, ptr] : scriptList)
        {
            uuidStr = UUID_To_String(uuid);

            if (!ptr->PreEncoding(data["ScriptList"][uuidStr]))
            {
                return false;
            }
        }

        return true;
    }

    bool ScriptSystem::PostEncoding(json& data) const
    {
        std::string uuidStr;
        for (auto& [uuid, ptr] : scriptList)
        {
            uuidStr = UUID_To_String(uuid);

            auto itr = data["ScriptList"].find(uuidStr);
            if (itr == data["ScriptList"].end())
            {
                return false;
            }

            if (!ptr->PostEncoding(itr.value()))
            {
                return false;
            }
        }

        return true;
    }

    bool ScriptSystem::PreDecoding(const json& data)
    {
        if (!data.contains("ScriptList"))
            return true;
        UUID uuid;
        for (auto& [uuidStr, scriptData] : data["ScriptList"].items())
        {
            uuid = String_To_UUID(uuidStr);

            auto script = CreateScript();

            if (script == nullptr)
            {
                Clear();
                return false;
            }

            scriptList.erase(script->GetUUID());

            // UUID
            script->SetUUID(uuid);
            scriptList[uuid] = script;

            if (!script->PreDecoding(scriptData))
            {
                Clear();
                return false;
            }
        }

        return true;
    }

    bool ScriptSystem::PostDecoding(const json& data)
    {
        if (!data.contains("ScriptList"))
            return true;
        UUID uuid;
        for (auto& [uuidStr, scriptData] : data["ScriptList"].items())
        {
            uuid = String_To_UUID(uuidStr);

            for (auto& each : scriptList)
            {
                if (each.first == uuid)
                {
                    if (!each.second->PostDecoding(scriptData))
                    {
                        Clear();
                        return false;
                    }

                    break;
                }
            }
        }

        return true;
    }
}
