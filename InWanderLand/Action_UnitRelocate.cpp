#include "Action_UnitRelocate.h"
#include "InWanderLand.h"

namespace application
{
    Action_UnitRelocate::~Action_UnitRelocate()
    {
        if (targetUnit)
        {
            targetUnit->RemoveObserver(this);
        }

        if (destinationUnit)
        {
            destinationUnit->RemoveObserver(this);
            editor::InstanceManager::GetSingletonInstance().DeleteInstance(destinationUnit->GetUUID());
        }
    }

    CoroutineObject<void> Action_UnitRelocate::DoAction()
    {
        if (targetUnit->inGameUnit.expired())
        {
            co_return;
        }
        auto ts = targetUnit->inGameUnit.lock()->GetTransform();
        auto startPos = ts->GetWorldPosition();

        Vector3d endPos = { destinationUnit->pod.position.x, destinationUnit->pod.position.y, destinationUnit->pod.position.z };

        endPos.y = 0;

        targetUnit->inGameUnit.lock()->Relocate(endPos);
        co_return;
    }

    bool Action_UnitRelocate::IsValid()
    {
        return (targetUnit == nullptr) ? false : (destinationUnit == nullptr) ? false : true;
    }

    void Action_UnitRelocate::SetTargetUnit(editor::UnitData* unit)
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

        if (destinationUnit)
        {
            editor::InstanceManager::GetSingletonInstance().DeleteInstance(destinationUnit->GetUUID());
        }
    }

    void Action_UnitRelocate::SetDestinationUnit(editor::UnitData* unit)
    {
        if (destinationUnit)
        {
            destinationUnit->RemoveObserver(this);
        }

        destinationUnit = unit;
        if (unit)
        {
            unit->RegisterObserver(this);
            if (destinationUnit->GetPaletteInstance())
            {
                unit->pod.isGuide = true;
                static_cast<editor::palette::UnitEditorInstance*>(destinationUnit->GetPaletteInstance())->ChangeGuideInstance();
            }
        }
    }

    void Action_UnitRelocate::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
    {
        switch (event)
        {
        case application::ObservationEvent::Destroy:
        {
            if (targetUnit == static_cast<editor::UnitData*>(target))
            {
                targetUnit = nullptr;
            }
            if (destinationUnit == static_cast<editor::UnitData*>(target))
            {
                destinationUnit = nullptr;
            }
            break;
        }
        default:
            break;
        }
    }

    void Action_UnitRelocate::ImGui_DrawDataPopup(Action_UnitRelocate* data)
    {
        if (ImGui::MenuItem("SetTargetUnit(Relocate)"))
        {
            editor::EditorLayer::SetInputControl(false);
            editor::imgui::ShowMessageBox("SetTargetUnit(Relocate)", [data]()
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
                        editor::imgui::CloseMessageBox("SetTargetUnit(Relocate)");
                        editor::EditorLayer::SetInputControl(true);
                        editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitRelocate>("SetTargetUnit(Relocate)", data);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetUnit(Relocate)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }

        if (data->destinationUnit)
        {
            if (ImGui::MenuItem("EditDestinationUnit(Relocate)"))
            {
                editor::EditorLayer::SetInputControl(false);
                editor::imgui::ShowMessageBox("EditDestinationUnit(Relocate)", [data]()
                    {
                        editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                        ImGui::Separator();

                        ImGui::SetNextItemWidth(-1);

                        ImGui::Text(data->destinationUnit->pod.templateData->pod.skinnedFBXName.c_str());

                        ImGui::Separator();

                        if (ImGui::Button("Edit"))
                        {
                            data->destinationUnit->GetPaletteInstance()->GetGameObject()->SetSelfActive(true);
                            ImGui::CloseCurrentPopup();
                            editor::imgui::CloseMessageBox("EditDestinationUnit(Relocate)");
                            editor::EditorLayer::SetInputControl(true);
                            editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitRelocate>("EditDestinationUnit(Relocate)", data);
                        }
                        ImGui::SameLine();

                        if (ImGui::Button("Cancel"))
                        {
                            ImGui::CloseCurrentPopup();
                            editor::imgui::CloseMessageBox("EditDestinationUnit(Relocate)");
                            editor::EditorLayer::SetInputControl(true);
                        }
                    }, 300);
            }
        }
        else
        {
            if (data->targetUnit && ImGui::MenuItem("SetDestinationUnit(Relocate)"))
            {
                editor::EditorLayer::SetInputControl(false);
                editor::imgui::ShowMessageBox("SetDestinationUnit(Relocate)", [data]()
                    {
                        if (data->destinationUnit)
                        {
                            ImGui::CloseCurrentPopup();
                            editor::imgui::CloseMessageBox("SetDestinationUnit(Relocate)");
                            editor::EditorLayer::SetInputControl(true);
                            return;
                        }

                        editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                        ImGui::Separator();

                        ImGui::SetNextItemWidth(-1);

                        ImGui::Text("------");

                        ImGui::Separator();

                        if (ImGui::Button("Edit"))
                        {
                            ImGui::CloseCurrentPopup();
                            editor::imgui::CloseMessageBox("SetDestinationUnit(Relocate)");
                            editor::EditorLayer::SetInputControl(true);
                            editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_UnitRelocate>("SetDestinationUnit(Relocate)", data);
                        }
                        ImGui::SameLine();

                        if (ImGui::Button("Cancel"))
                        {
                            ImGui::CloseCurrentPopup();
                            editor::imgui::CloseMessageBox("SetDestinationUnit(Relocate)");
                            editor::EditorLayer::SetInputControl(true);
                        }
                    }, 300);
            }
        }
    }

    bool Action_UnitRelocate::PreEncoding(json& data) const
    {
        return true;
    }

    bool Action_UnitRelocate::PostEncoding(json& data) const
    {
        data["targetUnit"] = targetUnit ? UUID_To_String(targetUnit->GetUUID()) : "nullptr";
        data["destinationUnit"] = destinationUnit ? UUID_To_String(destinationUnit->GetUUID()) : "nullptr";
        return true;
    }

    bool Action_UnitRelocate::PreDecoding(const json& data)
    {
        return true;
    }

    bool Action_UnitRelocate::PostDecoding(const json& data)
    {
        SetTargetUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["targetUnit"])));
        SetDestinationUnit(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::UnitData*>(String_To_UUID(data["destinationUnit"])));
        return true;
    }
}
