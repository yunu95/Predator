#include "Action_BlockPlayerSwitch.h"
#include "LocalTimeEntityManager.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_BlockPlayerSwitch::Action_BlockPlayerSwitch(bool block)
    {
        this->block = block;
    }

    CoroutineObject<void> Action_BlockPlayerSwitch::DoAction()
    {
        UIManager::Instance().SetPortraitsClickable(!block);
        co_return;
    }

    void Action_BlockPlayerSwitch::SetBlock(bool block)
    {
        this->block = block;
    }

    void Action_BlockPlayerSwitch::ImGui_DrawDataPopup(Action_BlockPlayerSwitch* data)
    {
        if (ImGui::MenuItem("SetBlockUnblock"))
        {
            editor::EditorLayer::SetInputControl(false);
            static bool block = true;
            block = data->block;
            editor::imgui::ShowMessageBox("SetBlockUnblock", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("##Block", &block);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetBlock(block);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetBlockUnblock");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetBlockUnblock");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_BlockPlayerSwitch::PreEncoding(json& data) const
    {
        data["block"] = block;
        return true;
    }

    bool Action_BlockPlayerSwitch::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_BlockPlayerSwitch::PreDecoding(const json& data)
    {
        block = data["block"];
        return true;
    }

    bool Action_BlockPlayerSwitch::PostDecoding(const json& data)
    {
        return true;
    }
}