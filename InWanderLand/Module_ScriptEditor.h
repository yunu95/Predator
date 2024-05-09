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
