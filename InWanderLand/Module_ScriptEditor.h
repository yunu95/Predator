/// 2024. 04. 11 김상준
/// Script 편집을 위한 Editor Module

#pragma once

#include "Singleton.h"
#include "EditorModule.h"

#include "ScriptSystem.h"
#include "TriggerList.h"
#include "ConditionList.h"
#include "ActionList.h"

#include "imgui.h"
#include "imgui_ModuleData.h"

#include <memory>

namespace application
{
    class Script;
}

namespace application
{
    namespace editor
    {
        class Module_ScriptEditor
            : public EditorModule, public Singleton<Module_ScriptEditor>
        {
            friend class Singleton<Module_ScriptEditor>;

        public:
            virtual ~Module_ScriptEditor();

            virtual void Initialize() override;
            virtual void Update(float ts) override;
            virtual void GUIProgress() override;
            virtual void Finalize() override;

            void Clear();

            Script* GetSelectedScript();

        private:
            Module_ScriptEditor();

            // Script 리스트와 선택한 리스트에 대한 내용을 출력하는 두 패널을 그려주는 함수
            void DrawLayout();
            // Script 리스트를 그리는 함수
            void DrawList(const ImVec2& region);
            // 선택한 Script 의 내용을 출력하는 패널을 그리는 함수
            void DrawScriptPanel(const ImVec2& region);
            // Script 내용을 출력하는 함수
            void DrawScript(Script* data);

            void ImGui_AddTriggerPopup();
            void ImGui_AddConditionPopup();
            void ImGui_AddActionPopup();

            void DrawTrigger(std::shared_ptr<ITrigger> data);
            void DrawCondition(std::shared_ptr<ICondition> data);
            void DrawAction(std::shared_ptr<IAction> data);

            template <typename T> requires std::is_same_v<ITrigger, T> || std::is_same_v<ICondition, T> || std::is_same_v<IAction, T>
            void PopUpDataEdit(T* data)
            {

            }

            template <>
            void PopUpDataEdit(ITrigger* data)
            {
                auto type = data->GetType();
                if (ImGui::BeginPopup(data->GetTypeName().c_str()))
                {
                    switch (type)
                    {
                    case application::TriggerType::GameStart:
                    {
                        Trigger_GameStart::ImGui_DrawDataPopup(static_cast<Trigger_GameStart*>(data));
                        break;
                    }
                    case application::TriggerType::EnterRegion:
                    {
                        Trigger_EnterRegion::ImGui_DrawDataPopup(static_cast<Trigger_EnterRegion*>(data));
                        break;
                    }
                    case application::TriggerType::LeaveRegion:
                    {
                        Trigger_LeaveRegion::ImGui_DrawDataPopup(static_cast<Trigger_LeaveRegion*>(data));
                        break;
                    }
                    case application::TriggerType::RepeatPeriodically:
                    {
                        Trigger_RepeatPeriodically::ImGui_DrawDataPopup(static_cast<Trigger_RepeatPeriodically*>(data));
                        break;
                    }
                    case application::TriggerType::RepeatPeriodicallyRealTime:
                    {
                        Trigger_RepeatPeriodicallyRealTime::ImGui_DrawDataPopup(static_cast<Trigger_RepeatPeriodicallyRealTime*>(data));
                        break;
                    }
                    case application::TriggerType::UnitAppear:
                    {
                        Trigger_UnitAppear::ImGui_DrawDataPopup(static_cast<Trigger_UnitAppear*>(data));
                        break;
                    }
                    case application::TriggerType::UnitDie:
                    {
                        Trigger_UnitDie::ImGui_DrawDataPopup(static_cast<Trigger_UnitDie*>(data));
                        break;
                    }
                    case application::TriggerType::InteractableTriggerEnter:
                    {
                        Trigger_InteractableTriggerEnter::ImGui_DrawDataPopup(static_cast<Trigger_InteractableTriggerEnter*>(data));
                        break;
                    }
                    case application::TriggerType::InteractableTriggerExit:
                    {
                        Trigger_InteractableTriggerExit::ImGui_DrawDataPopup(static_cast<Trigger_InteractableTriggerExit*>(data));
                        break;
                    }
                    case application::TriggerType::StartWave:
                    {
                        Trigger_StartWave::ImGui_DrawDataPopup(static_cast<Trigger_StartWave*>(data));
                        break;
                    }
                    case application::TriggerType::EndWave:
                    {
                        Trigger_EndWave::ImGui_DrawDataPopup(static_cast<Trigger_EndWave*>(data));
                        break;
                    }
                    case application::TriggerType::StageStart:
                    {
                        Trigger_StageStart::ImGui_DrawDataPopup(static_cast<Trigger_StageStart*>(data));
                        break;
                    }
                    default:
                        break;
                    }
                    ImGui::EndPopup();
                }
            }

            template <>
            void PopUpDataEdit(ICondition* data)
            {
                auto type = data->GetType();
                if (ImGui::BeginPopup(data->GetTypeName().c_str()))
                {
                    switch (type)
                    {
                    case application::ConditionType::CinematicModeOn:
                    {
                        Condition_CinematicModeOn::ImGui_DrawDataPopup(static_cast<Condition_CinematicModeOn*>(data));
                        break;
                    }
                    case application::ConditionType::CinematicModeOff:
                    {
                        Condition_CinematicModeOff::ImGui_DrawDataPopup(static_cast<Condition_CinematicModeOff*>(data));
                        break;
                    }
                    case application::ConditionType::TutorialModeOn:
                    {
                        Condition_TutorialModeOn::ImGui_DrawDataPopup(static_cast<Condition_TutorialModeOn*>(data));
                        break;
                    }
                    case application::ConditionType::TutorialModeOff:
                    {
                        Condition_TutorialModeOff::ImGui_DrawDataPopup(static_cast<Condition_TutorialModeOff*>(data));
                        break;
                    }
                    case application::ConditionType::TacticModeOn:
                    {
                        Condition_TacticModeOn::ImGui_DrawDataPopup(static_cast<Condition_TacticModeOn*>(data));
                        break;
                    }
                    case application::ConditionType::TacticModeOff:
                    {
                        Condition_TacticModeOff::ImGui_DrawDataPopup(static_cast<Condition_TacticModeOff*>(data));
                        break;
                    }
                    case application::ConditionType::PlayerState:
                    {
                        Condition_PlayerState::ImGui_DrawDataPopup(static_cast<Condition_PlayerState*>(data));
                        break;
                    }
                    case application::ConditionType::OrnamentShow:
                    {
                        Condition_OrnamentShow::ImGui_DrawDataPopup(static_cast<Condition_OrnamentShow*>(data));
                        break;
                    }
                    case application::ConditionType::OrnamentHide:
                    {
                        Condition_OrnamentHide::ImGui_DrawDataPopup(static_cast<Condition_OrnamentHide*>(data));
                        break;
                    }
                    case application::ConditionType::Stage:
                    {
                        Condition_Stage::ImGui_DrawDataPopup(static_cast<Condition_Stage*>(data));
                        break;
                    }
                    default:
                        break;
                    }
                    ImGui::EndPopup();
                }
            }

            template <>
            void PopUpDataEdit(IAction* data)
            {
                auto type = data->GetType();
                if (ImGui::BeginPopup(data->GetTypeName().c_str()))
                {
                    switch (type)
                    {
                    case application::ActionType::WaitForSeconds:
                    {
                        Action_WaitForSeconds::ImGui_DrawDataPopup(static_cast<Action_WaitForSeconds*>(data));
                        break;
                    }
                    case application::ActionType::WaitForRealSeconds:
                    {
                        Action_WaitForRealSeconds::ImGui_DrawDataPopup(static_cast<Action_WaitForRealSeconds*>(data));
                        break;
                    }
                    case application::ActionType::CinematicModeChange:
                    {
                        Action_CinematicModeChange::ImGui_DrawDataPopup(static_cast<Action_CinematicModeChange*>(data));
                        break;
                    }
                    case application::ActionType::CameraChangeView:
                    {
                        Action_CameraChangeView::ImGui_DrawDataPopup(static_cast<Action_CameraChangeView*>(data));
                        break;
                    }
                    case application::ActionType::CameraSaveView:
                    {
                        Action_CameraSaveView::ImGui_DrawDataPopup(static_cast<Action_CameraSaveView*>(data));
                        break;
                    }
                    case application::ActionType::CameraLoadView:
                    {
                        Action_CameraLoadView::ImGui_DrawDataPopup(static_cast<Action_CameraLoadView*>(data));
                        break;
                    }
                    case application::ActionType::CinematicFadeIn:
                    {
                        Action_CinematicFadeIn::ImGui_DrawDataPopup(static_cast<Action_CinematicFadeIn*>(data));
                        break;
                    }
                    case application::ActionType::CinematicFadeOut:
                    {
                        Action_CinematicFadeOut::ImGui_DrawDataPopup(static_cast<Action_CinematicFadeOut*>(data));
                        break;
                    }
                    case application::ActionType::WaitPreviousActionEnd:
                    {
                        Action_WaitPreviousActionEnd::ImGui_DrawDataPopup(static_cast<Action_WaitPreviousActionEnd*>(data));
                        break;
                    }
                    case application::ActionType::CameraRevert:
                    {
                        Action_CameraRevert::ImGui_DrawDataPopup(static_cast<Action_CameraRevert*>(data));
                        break;
                    }
                    case application::ActionType::TutorialModeChange:
                    {
                        Action_TutorialModeChange::ImGui_DrawDataPopup(static_cast<Action_TutorialModeChange*>(data));
                        break;
                    }
                    case application::ActionType::PlayManualDialogue:
                    {
                        Action_PlayManualDialogue::ImGui_DrawDataPopup(static_cast<Action_PlayManualDialogue*>(data));
                        break;
                    }
                    case application::ActionType::PlayTimedDialogue:
                    {
                        Action_PlayTimedDialogue::ImGui_DrawDataPopup(static_cast<Action_PlayTimedDialogue*>(data));
                        break;
                    }
                    case application::ActionType::UnitMove:
                    {
                        Action_UnitMove::ImGui_DrawDataPopup(static_cast<Action_UnitMove*>(data));
                        break;
                    }
                    case application::ActionType::UnitRotate:
                    {
                        Action_UnitRotate::ImGui_DrawDataPopup(static_cast<Action_UnitRotate*>(data));
                        break;
                    }
                    case application::ActionType::UnitRescale:
                    {
                        Action_UnitRescale::ImGui_DrawDataPopup(static_cast<Action_UnitRescale*>(data));
                        break;
                    }
                    case application::ActionType::UnitMoveWithRotateAndRescale:
                    {
                        Action_UnitMoveWithRotateAndRescale::ImGui_DrawDataPopup(static_cast<Action_UnitMoveWithRotateAndRescale*>(data));
                        break;
                    }
                    case application::ActionType::UnitRelocate:
                    {
                        Action_UnitRelocate::ImGui_DrawDataPopup(static_cast<Action_UnitRelocate*>(data));
                        break;
                    }
                    case application::ActionType::UnitPauseAll:
                    {
                        Action_UnitPauseAll::ImGui_DrawDataPopup(static_cast<Action_UnitPauseAll*>(data));
                        break;
                    }
                    case application::ActionType::PlayerSelect:
                    {
                        Action_PlayerSelect::ImGui_DrawDataPopup(static_cast<Action_PlayerSelect*>(data));
                        break;
                    }
                    case application::ActionType::BlockPlayerSwitch:
                    {
                        Action_BlockPlayerSwitch::ImGui_DrawDataPopup(static_cast<Action_BlockPlayerSwitch*>(data));
                        break;
                    }
                    case application::ActionType::BlockSkillCancel:
                    {
                        Action_BlockSkillCancel::ImGui_DrawDataPopup(static_cast<Action_BlockSkillCancel*>(data));
                        break;
                    }
                    case application::ActionType::BlockSkillSelection:
                    {
                        Action_BlockSkillSelection::ImGui_DrawDataPopup(static_cast<Action_BlockSkillSelection*>(data));
                        break;
                    }
                    case application::ActionType::AwaitSkillSelection:
                    {
                        Action_AwaitSkillSelection::ImGui_DrawDataPopup(static_cast<Action_AwaitSkillSelection*>(data));
                        break;
                    }
                    case application::ActionType::AwaitSkillActivation:
                    {
                        Action_AwaitSkillActivation::ImGui_DrawDataPopup(static_cast<Action_AwaitSkillActivation*>(data));
                        break;
                    }
                    case application::ActionType::SetTimeScale:
                    {
                        Action_SetTimeScale::ImGui_DrawDataPopup(static_cast<Action_SetTimeScale*>(data));
                        break;
                    }
                    case application::ActionType::PauseWaves:
                    {
                        Action_PauseWaves::ImGui_DrawDataPopup(static_cast<Action_PauseWaves*>(data));
                        break;
                    }
                    case application::ActionType::EngageStage:
                    {
                        Action_EngageStage::ImGui_DrawDataPopup(static_cast<Action_EngageStage*>(data));
                        break;
                    }
                    case application::ActionType::SoundPlay:
                    {
                        Action_SoundPlay::ImGui_DrawDataPopup(static_cast<Action_SoundPlay*>(data));
                        break;
                    }
                    case application::ActionType::SoundPlayMusic:
                    {
                        Action_SoundPlayMusic::ImGui_DrawDataPopup(static_cast<Action_SoundPlayMusic*>(data));
                        break;
                    }
                    case application::ActionType::SoundSetMusicVolume:
                    {
                        Action_SoundSetMusicVolume::ImGui_DrawDataPopup(static_cast<Action_SoundSetMusicVolume*>(data));
                        break;
                    }
                    case application::ActionType::UISetActive:
                    {
                        Action_UISetActive::ImGui_DrawDataPopup(static_cast<Action_UISetActive*>(data));
                        break;
                    }
                    case application::ActionType::OrnamentMoveWithRotateAndRescale:
                    {
                        Action_OrnamentMoveWithRotateAndRescale::ImGui_DrawDataPopup(static_cast<Action_OrnamentMoveWithRotateAndRescale*>(data));
                        break;
                    }
                    case application::ActionType::OrnamentShow:
                    {
                        Action_OrnamentShow::ImGui_DrawDataPopup(static_cast<Action_OrnamentShow*>(data));
                        break;
                    }
                    case application::ActionType::OrnamentHide:
                    {
                        Action_OrnamentHide::ImGui_DrawDataPopup(static_cast<Action_OrnamentHide*>(data));
                        break;
                    }
                    case application::ActionType::OrnamentFloating:
                    {
                        Action_OrnamentFloating::ImGui_DrawDataPopup(static_cast<Action_OrnamentFloating*>(data));
                        break;
                    }
                    case application::ActionType::BossAppear:
                    {
                        Action_BossAppear::ImGui_DrawDataPopup(static_cast<Action_BossAppear*>(data));
                        break;
                    }
                    case application::ActionType::RequestState:
                    {
                        Action_RequestState::ImGui_DrawDataPopup(static_cast<Action_RequestState*>(data));
                        break;
                    }
                    case application::ActionType::AwaitSkillTargeting:
                    {
                        Action_AwaitSkillTargeting::ImGui_DrawDataPopup(static_cast<Action_AwaitSkillTargeting*>(data));
                        break;
                    }
                    case application::ActionType::AwaitSkillExpiration:
                    {
                        Action_AwaitSkillExpiration::ImGui_DrawDataPopup(static_cast<Action_AwaitSkillExpiration*>(data));
                        break;
                    }
                    case application::ActionType::ForceUnitPaused:
                    {
                        Action_ForceUnitPaused::ImGui_DrawDataPopup(static_cast<Action_ForceUnitPaused*>(data));
                        break;
                    }
                    case application::ActionType::ForceUnitUnpaused:
                    {
                        Action_ForceUnitUnpaused::ImGui_DrawDataPopup(static_cast<Action_ForceUnitUnpaused*>(data));
                        break;
                    }
                    case application::ActionType::SetZoomFactor:
                    {
                        Action_SetZoomFactor::ImGui_DrawDataPopup(static_cast<Action_SetZoomFactor*>(data));
                        break;
                    }
                    case application::ActionType::SetCamBidirectionalPath:
                    {
                        Action_SetCamBidirectionalPath::ImGui_DrawDataPopup(static_cast<Action_SetCamBidirectionalPath*>(data));
                        break;
                    }
                    case application::ActionType::ParticleMoveWithRotateAndRescale:
                    {
                        Action_ParticleMoveWithRotateAndRescale::ImGui_DrawDataPopup(static_cast<Action_ParticleMoveWithRotateAndRescale*>(data));
                        break;
                    }
                    case application::ActionType::ParticleShow:
                    {
                        Action_ParticleShow::ImGui_DrawDataPopup(static_cast<Action_ParticleShow*>(data));
                        break;
                    }
                    case application::ActionType::ParticleHide:
                    {
                        Action_ParticleHide::ImGui_DrawDataPopup(static_cast<Action_ParticleHide*>(data));
                        break;
                    }
                    case application::ActionType::UnitPlayAnimation:
                    {
                        Action_UnitPlayAnimation::ImGui_DrawDataPopup(static_cast<Action_UnitPlayAnimation*>(data));
                        break;
                    }
                    case application::ActionType::PullScriptTrigger:
                    {
                        Action_PullScriptTrigger::ImGui_DrawDataPopup(static_cast<Action_PullScriptTrigger*>(data));
                        break;
                    }
                    case application::ActionType::SetTacticCamera:
                    {
                        Action_SetTacticCamera::ImGui_DrawDataPopup(static_cast<Action_SetTacticCamera*>(data));
                        break;
                    }
                    default:
                        break;
                    }
                    ImGui::EndPopup();
                }
            }

            Script* selectedScript = nullptr;
            std::shared_ptr<ITrigger> selectedTrigger = nullptr;
            std::shared_ptr<ICondition> selectedCondition = nullptr;
            std::shared_ptr<IAction> selectedAction = nullptr;
            bool isEditingPopup = false;
        };
    }
}
