#include "Trigger_Unit.h"
#include "UnitData.h"

#include "Unit.h"

#include "Application.h"
#include "EditorLayer.h"

#include "EditorPopupManager.h"

namespace application
{
    Trigger_UnitAppear::~Trigger_UnitAppear()
    {
        if (targetUnit)
        {
            targetUnit->RemoveObserver(this);
        }
    }

    void Trigger_UnitAppear::LinkCallback()
    {
        targetUnit->onCreated.AddVolatileCallback([=]() { PullTrigger(); });
    }

    void Trigger_UnitAppear::SetUnit(editor::UnitData* unit)
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

    bool Trigger_UnitAppear::IsValid()
    {
        return (targetUnit != nullptr) ? true : false;
    }

    void Trigger_UnitAppear::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
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

    void Trigger_UnitAppear::ImGui_DrawDataPopup(Trigger_UnitAppear* data)
    {
        if (ImGui::MenuItem("SetUnitAppear"))
        {
            editor::EditorLayer::SetInputControl(false);
            editor::imgui::ShowMessageBox("SetUnitAppear", [data]()
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
                        editor::imgui::CloseMessageBox("SetUnitAppear");
                        editor::EditorLayer::SetInputControl(true);
                        editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Trigger_UnitAppear>("SetUnitAppear", data);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetUnitAppear");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Trigger_UnitAppear::PreEncoding(json& data) const
    {
        return true;
    }

    bool Trigger_UnitAppear::PostEncoding(json& data) const
    {
        data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
        return true;
    }

    bool Trigger_UnitAppear::PreDecoding(const json& data)
    {
        return true;
    }

    bool Trigger_UnitAppear::PostDecoding(const json& data)
    {
        SetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
        return true;
    }

    Trigger_UnitDie::~Trigger_UnitDie()
    {
        if (targetUnit)
        {
            targetUnit->RemoveObserver(this);
        }
    }

    void Trigger_UnitDie::LinkCallback()
    {
        targetUnit->onStateEngage[UnitBehaviourTree::Death].AddVolatileCallback([=]() { PullTrigger(); });
    }

    void Trigger_UnitDie::SetUnit(editor::UnitData* unit)
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

    bool Trigger_UnitDie::IsValid()
    {
        return (targetUnit != nullptr) ? true : false;
    }

    void Trigger_UnitDie::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
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

    void Trigger_UnitDie::ImGui_DrawDataPopup(Trigger_UnitDie* data)
    {
        if (ImGui::MenuItem("SetUnitDie"))
        {
            editor::EditorLayer::SetInputControl(false);
            editor::imgui::ShowMessageBox("SetUnitDie", [data]()
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
                        editor::imgui::CloseMessageBox("SetUnitDie");
                        editor::EditorLayer::SetInputControl(true);
                        editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Trigger_UnitDie>("SetUnitDie", data);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetUnitDie");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Trigger_UnitDie::PreEncoding(json& data) const
    {
        return true;
    }

    bool Trigger_UnitDie::PostEncoding(json& data) const
    {
        data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
        return true;
    }

    bool Trigger_UnitDie::PreDecoding(const json& data)
    {
        return true;
    }

    bool Trigger_UnitDie::PostDecoding(const json& data)
    {
        SetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
        return true;
    }
}
