#include "Action_SetPlayerConstrainingRegion.h"

#include "InWanderLand.h"
#include "RegionData.h"
#include "RegionEditorInstance.h"

#include "Application.h"
#include "EditorLayer.h"

#include "EditorPopupManager.h"
#include "InstanceManager.h"
#include "Panel_Palette.h"
#include "RegionPalette.h"

#include "YunutyEngine.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace application
{
    Action_SetPlayerConstrainingRegion::~Action_SetPlayerConstrainingRegion()
    {
        if (targetRegion)
        {
            targetRegion->RemoveObserver(this);
        }
    }

    CoroutineObject<void> Action_SetPlayerConstrainingRegion::DoAction()
    {
        if (doesConstrain)
        {
            if (targetRegion)
            {
                PlaytimeRegion::playerConstrainingRegion = targetRegion->playtimeRegion->GetWeakPtr<PlaytimeRegion>();
            }
        }
        else
        {
            PlaytimeRegion::playerConstrainingRegion.reset();
        }
        co_return;
    }

    bool Action_SetPlayerConstrainingRegion::IsValid()
    {
        return (targetRegion == nullptr) ? false : true;
    }

    void Action_SetPlayerConstrainingRegion::SetTargetRegion(editor::RegionData* region)
    {
        if (targetRegion)
        {
            targetRegion->RemoveObserver(this);
        }

        targetRegion = region;
        if (region)
        {
            region->RegisterObserver(this);
        }
    }

    void Action_SetPlayerConstrainingRegion::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
    {
        switch (event)
        {
        case application::ObservationEvent::Destroy:
        {
            if (targetRegion == static_cast<editor::RegionData*>(target))
            {
                targetRegion = nullptr;
            }
            break;
        }
        default:
            break;
        }
    }

    void Action_SetPlayerConstrainingRegion::ImGui_DrawDataPopup(Action_SetPlayerConstrainingRegion* data)
    {
        if (ImGui::MenuItem("SetTargetRegion(SetAsNavObstacle)"))
        {
            editor::EditorLayer::SetInputControl(false);
            editor::imgui::ShowMessageBox("SetTargetRegion(SetAsNavObstacle)", [data]()
                {
                    auto& pp = editor::PalettePanel::GetSingletonInstance();
                    auto& rp = editor::palette::RegionPalette::SingleInstance();

                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    if (data->targetRegion)
                    {
                        ImGui::Text(yutility::GetString(data->targetRegion->pod.name).c_str());
                        pp.ChangeTab("Region");
                        rp.Reset();
                        rp.SelectRegion(data->targetRegion);
                    }
                    else
                    {
                        ImGui::Text("------");
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Edit"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetRegion(SetAsNavObstacle)");
                        editor::EditorLayer::SetInputControl(true);
                        editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_SetPlayerConstrainingRegion>("SetTargetRegion", data);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetRegion(SetAsNavObstacle)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_SetPlayerConstrainingRegion::PreEncoding(json& data) const
    {
        data["doesConstrain"] = doesConstrain;
        return true;
    }

    bool Action_SetPlayerConstrainingRegion::PostEncoding(json& data) const
    {
        data["targetRegion"] = targetRegion ? UUID_To_String(targetRegion->GetUUID()) : "nullptr";
        return true;
    }

    bool Action_SetPlayerConstrainingRegion::PreDecoding(const json& data)
    {
        doesConstrain = data["doesConstrain"];
        return true;
    }

    bool Action_SetPlayerConstrainingRegion::PostDecoding(const json& data)
    {
        SetTargetRegion(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::RegionData*>(String_To_UUID(data["targetRegion"])));
        return true;
    }
}
