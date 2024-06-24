#include "Action_ForceUnitUnpaused.h"
#include "LocalTimeEntityManager.h"
#include "Skill.h"
#include "DelegateCallback.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    CoroutineObject<void> Action_ForceUnitUnpaused::DoAction()
    {
        bool proceed = false;
        if (auto ingameUnit = targetUnit->inGameUnit.lock())
        {
            ingameUnit->pauseRequested = !withdrawRequest;
        }
        co_return;
    };

    void Action_ForceUnitUnpaused::SetTargetUnit(editor::UnitData* unit)
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
    void Action_ForceUnitUnpaused::ImGui_DrawDataPopup(Action_ForceUnitUnpaused* data)
    {
        if (ImGui::MenuItem("SetTargetUnit(ForceUnpause)"))
        {
            editor::EditorLayer::SetInputControl(false);
            editor::imgui::ShowMessageBox("SetTargetUnit(ForceUnpause)", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    if (data->targetUnit)
                    {
                        ImGui::Text(data->targetUnit->pod.templateData->pod.skinnedFBXName.c_str());
                    }
                    else
                    {
                        ImGui::Text("------");
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Edit"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetUnit(ForceUnpause)");
                        editor::EditorLayer::SetInputControl(true);
                        editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_ForceUnitUnpaused>("SetTargetUnit(ForceUnpause)", data);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetUnit(ForceUnpause)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_ForceUnitUnpaused::PreEncoding(json& data) const
    {
        data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
        data["withdrawRequest"] = withdrawRequest;
        return true;
    }

    bool Action_ForceUnitUnpaused::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_ForceUnitUnpaused::PreDecoding(const json& data)
    {
        withdrawRequest = data["withdrawRequest"];
        return true;
    }

    bool Action_ForceUnitUnpaused::PostDecoding(const json& data)
    {
        SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
        return true;
    }
}