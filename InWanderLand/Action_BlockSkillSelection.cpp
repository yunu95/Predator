#include "Action_BlockSkillSelection.h"
#include "LocalTimeEntityManager.h"
#include "Skill.h"
#include "DelegateCallback.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_BlockSkillSelection::Action_BlockSkillSelection(SkillType::Enum skillType)
    {
        this->skillType.enumValue = skillType;
        this->skillType.enumName = POD_Enum<SkillType::Enum>::GetEnumNameMap().at(skillType);
    };

    CoroutineObject<void> Action_BlockSkillSelection::DoAction()
    {
        if (!applyExceptTarget)
        {
            PlayerController::Instance().blockSkillSelection[skillType.enumValue] = blocking;
        }
        else
        {
            for (auto i = (int)SkillType::NONE; i < SkillType::SKILL_NUM; i++)
            {
                if (i == skillType.enumValue)
                {
                    continue;
                }
                PlayerController::Instance().blockSkillSelection[(int)i] = blocking;
            }
        }
        co_return;
    };

    void Action_BlockSkillSelection::ImGui_DrawDataPopup(Action_BlockSkillSelection* data)
    {
        if (ImGui::MenuItem("SetSkillType"))
        {
            editor::EditorLayer::SetInputControl(false);
            static POD_Enum<SkillType::Enum> skillType = POD_Enum<SkillType::Enum>();
            static bool block = false;
            static bool applyExceptTarget = false;
            skillType = data->skillType;
            block = data->blocking;
            applyExceptTarget = data->applyExceptTarget;
            editor::imgui::ShowMessageBox("SetSkillType(BlockSkillSelection)", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("applyExceptTarget##applyExceptTarget", &applyExceptTarget);
                    
                    if (ImGui::BeginTable("##BlockSkillSelection", 2, ImGuiTableFlags_SizingStretchSame))
                    {
                        imgui::data::DrawData("SkillType", skillType);
                        ImGui::EndTable();
                    }

                    ImGui::Checkbox("blocking##blocking", &block);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->applyExceptTarget = applyExceptTarget;
                        data->skillType = skillType;
                        data->blocking = block;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSkillType(BlockSkillSelection)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSkillType(BlockSkillSelection)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_BlockSkillSelection::PreEncoding(json& data) const
    {
        data["skillType"] = skillType.enumValue;
        data["blocking"] = blocking;
        data["applyExceptTarget"] = applyExceptTarget;
        return true;
    }

    bool Action_BlockSkillSelection::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_BlockSkillSelection::PreDecoding(const json& data)
    {
        skillType.enumValue = data["skillType"];
        skillType.enumName = POD_Enum<SkillType::Enum>::GetEnumNameMap().at(skillType.enumValue);
        blocking = data["blocking"];
        applyExceptTarget = data["applyExceptTarget"];
        return true;
    }

    bool Action_BlockSkillSelection::PostDecoding(const json& data)
    {
        return true;
    }
}