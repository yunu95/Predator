#include "Trigger_Unit.h"
#include "UnitData.h"

#include "Unit.h"

#include "Application.h"
#include "EditorLayer.h"
#include "Panel_Palette.h"
#include "UnitPalette.h"

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
        if (!targetUnit->inGameUnit.expired())
        {
            targetUnit->inGameUnit.lock()->onCreated.AddVolatileCallback([=]() { PullTrigger(); });
        }
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
		if (ImGui::MenuItem("SetTargetUnit(UnitAppear)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetUnit(UnitAppear)", [data]()
				{
                    auto& pp = editor::PalettePanel::GetSingletonInstance();
                    auto& up = editor::palette::UnitPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

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
						editor::imgui::CloseMessageBox("SetTargetUnit(UnitAppear)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Trigger_UnitAppear>("SetTargetUnit(UnitAppear)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(UnitAppear)");
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
        if (!targetUnit->inGameUnit.expired())
        {
            targetUnit->inGameUnit.lock()->OnStateEngageCallback()[UnitBehaviourTree::Death].AddVolatileCallback([=]() { PullTrigger(); });
        }
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
		if (ImGui::MenuItem("SetTargetUnit(UnitDie)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetUnit(UnitDie)", [data]()
				{
                    auto& pp = editor::PalettePanel::GetSingletonInstance();
                    auto& up = editor::palette::UnitPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

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
						editor::imgui::CloseMessageBox("SetTargetUnit(UnitDie)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Trigger_UnitDie>("SetTargetUnit(UnitDie)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetUnit(UnitDie)");
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
