#include "Action_SetRegionAsNavObstacle.h"

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
    Action_SetRegionAsNavObstacle::~Action_SetRegionAsNavObstacle()
    {
        if (targetRegion)
        {
            targetRegion->RemoveObserver(this);
        }
    }

    CoroutineObject<void> Action_SetRegionAsNavObstacle::DoAction()
    {
        if (targetRegion)
        {
            targetRegion->playtimeRegion->ApplyAsObstacle(setAsObstacle);
        }
        co_return;
    }

    bool Action_SetRegionAsNavObstacle::IsValid()
    {
        return (targetRegion == nullptr) ? false : true;
    }

    void Action_SetRegionAsNavObstacle::SetTargetRegion(editor::RegionData* region)
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

    void Action_SetRegionAsNavObstacle::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
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

    void Action_SetRegionAsNavObstacle::ImGui_DrawDataPopup(Action_SetRegionAsNavObstacle* data)
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
                        editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_SetRegionAsNavObstacle>("SetTargetRegion(SetAsNavObstacle)", data);
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
        if (ImGui::MenuItem("SetAsObstacle(SetRegionAsNavObstacle)"))
        {
            static bool setAsObstacle;
            setAsObstacle = data->setAsObstacle;
            editor::imgui::ShowMessageBox("SetAsObstacle(SetRegionAsNavObstacle)", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("SetAsObstacle##", &setAsObstacle);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->setAsObstacle = setAsObstacle;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetAsObstacle(SetRegionAsNavObstacle)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetAsObstacle(SetRegionAsNavObstacle)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_SetRegionAsNavObstacle::PreEncoding(json& data) const
    {
        data["setAsObstacle"] = setAsObstacle;
        return true;
    }

    bool Action_SetRegionAsNavObstacle::PostEncoding(json& data) const
    {
        data["targetRegion"] = targetRegion ? UUID_To_String(targetRegion->GetUUID()) : "nullptr";
        return true;
    }

    bool Action_SetRegionAsNavObstacle::PreDecoding(const json& data)
    {
        setAsObstacle = data["setAsObstacle"];
        return true;
    }

    bool Action_SetRegionAsNavObstacle::PostDecoding(const json& data)
    {
        SetTargetRegion(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::RegionData*>(String_To_UUID(data["targetRegion"])));
        return true;
    }
}
