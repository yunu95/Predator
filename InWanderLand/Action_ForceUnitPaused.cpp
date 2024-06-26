#include "Action_ForceUnitPaused.h"
#include "LocalTimeEntityManager.h"
#include "Skill.h"
#include "DelegateCallback.h"
#include "Panel_Palette.h"
#include "UnitPalette.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    CoroutineObject<void> Action_ForceUnitPaused::DoAction()
    {
        bool proceed = false;
        if (auto ingameUnit = targetUnit->inGameUnit.lock())
        {
            ingameUnit->pauseRequested = !withdrawRequest;
        }
        co_return;
    };

    void Action_ForceUnitPaused::SetTargetUnit(editor::UnitData* unit)
    {
        if (targetUnit)
        {
            targetUnit->RemoveObserver(this);
        }

        targetUnit = unit;
        if (unit)
        {
            unit->RegisterObserver(this);
        }
    }
    void Action_ForceUnitPaused::ImGui_DrawDataPopup(Action_ForceUnitPaused* data)
    {
        if (ImGui::MenuItem("SetTargetUnit(ForcePause)"))
        {
            editor::EditorLayer::SetInputControl(false);
            editor::imgui::ShowMessageBox("SetTargetUnit(ForcePause)", [data]()
                {
                    auto& pp = editor::PalettePanel::GetSingletonInstance();
                    auto& up = editor::palette::UnitPalette::SingleInstance();

                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("withdrawRequest##withdrawRequest", &data->withdrawRequest);

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    if (data->targetUnit)
                    {
                        ImGui::Text(data->targetUnit->pod.templateData->pod.skinnedFBXName.c_str());
                        pp.ChangeTab("Unit");
                        up.Reset();
                        up.SelectUnitInstance(data->targetUnit);
                    }
                    else
                    {
                        ImGui::Text("------");
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Edit"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetUnit(ForcePause)");
                        editor::EditorLayer::SetInputControl(true);
                        editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_ForceUnitPaused>("SetTargetUnit(ForcePause)", data);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetUnit(ForcePause)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_ForceUnitPaused::PreEncoding(json& data) const
    {
        return true;
    }

    bool Action_ForceUnitPaused::PostEncoding(json& data) const
    {
        data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
        data["withdrawRequest"] = withdrawRequest;
        return true;
    }

    bool Action_ForceUnitPaused::PreDecoding(const json& data)
    {
        return true;
    }

    bool Action_ForceUnitPaused::PostDecoding(const json& data)
    {
        withdrawRequest = data["withdrawRequest"];
        //if (data.contains("targetUnit"))
        SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
        return true;
    }
    bool Action_ForceUnitPaused::IsValid()
    {
        return (targetUnit == nullptr) ? false : true;
    }

    void Action_ForceUnitPaused::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
    {
        switch (event)
        {
        case application::ObservationEvent::Destroy:
        {
            if (targetUnit == static_cast<editor::UnitData*>(target))
            {
                targetUnit = nullptr;
            }
            break;
        }
        default:
            break;
        }
    }
}