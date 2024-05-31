#include "Action_BlockSkillSelection.h"
#include "LocalTimeEntityManager.h"
#include "Skill.h"
#include "DelegateCallback.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_BlockSkillSelection::Action_BlockSkillSelection(SkillType skillType)
    {
        this->skillType = skillType;
    };

    CoroutineObject<void> Action_BlockSkillSelection::DoAction()
    {
        if (!applyExceptTarget)
        {
            PlayerController::Instance().blockSkillSelection[(int)(SkillType)skillType] = blocking;
        }
        else
        {
            for (auto i = (int)SkillType::NONE; i < (int)SkillType::SKILL_NUM; i++)
            {
                if (i == (int)(SkillType)skillType)
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
            static POD_Enum<SkillType> skillType;
            static bool block;
            static bool applyExceptTarget;
            skillType = data->skillType;
            block = data->blocking;
            applyExceptTarget = data->applyExceptTarget;
            editor::imgui::ShowMessageBox("SetSkillType", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("applyExceptTarget##applyExceptTarget", &applyExceptTarget);
                    imgui::data::DrawData("SkillType", skillType);
                    ImGui::Checkbox("blocking##blocking", &block);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->applyExceptTarget = applyExceptTarget;
                        data->skillType = skillType;
                        data->blocking = block;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSkillType");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSkillType");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_BlockSkillSelection::PreEncoding(json& data) const
    {
        data["skillType"] = (int)(SkillType)skillType;
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
        if (data.contains("index"))
        {
            skillType = (SkillType)data["index"].get<int>();
        }
        else
        {
            skillType = (SkillType)data["skillType"].get<int>();
        }
        blocking = data["blocking"];
        applyExceptTarget = applyExceptTarget["applyExceptTarget"];
        return true;
    }

    bool Action_BlockSkillSelection::PostDecoding(const json& data)
    {
        return true;
    }
}