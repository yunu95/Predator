#include "Action_BlockSkillCancel.h"
#include "LocalTimeEntityManager.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_BlockSkillCancel::Action_BlockSkillCancel(bool block)
    {
        this->block = block;
    }

    CoroutineObject<void> Action_BlockSkillCancel::DoAction()
    {
        // 여기에 스킬 취소를 못하게 막는 코드를 넣어야 한다.
        PlayerController::Instance().blockSkillCancel = block;
        co_return;
    }

    void Action_BlockSkillCancel::SetBlock(bool block)
    {
        this->block = block;
    }

    void Action_BlockSkillCancel::ImGui_DrawDataPopup(Action_BlockSkillCancel* data)
    {
        if (ImGui::MenuItem("SetBlockUnblock"))
        {
            editor::EditorLayer::SetInputControl(false);
            static bool block = true;
            block = data->block;
            editor::imgui::ShowMessageBox("SetBlockUnblock(SkillCancel)", [data]()
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
                        editor::imgui::CloseMessageBox("SetBlockUnblock(SkillCancel)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetBlockUnblock(SkillCancel)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_BlockSkillCancel::PreEncoding(json& data) const
    {
        data["block"] = block;
        return true;
    }

    bool Action_BlockSkillCancel::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_BlockSkillCancel::PreDecoding(const json& data)
    {
        block = data["block"];
        return true;
    }

    bool Action_BlockSkillCancel::PostDecoding(const json& data)
    {
        return true;
    }
}