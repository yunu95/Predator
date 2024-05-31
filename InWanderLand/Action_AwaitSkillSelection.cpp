#include "Action_AwaitSkillSelection.h"
#include "LocalTimeEntityManager.h"
#include "Skill.h"
#include "SkillType.h"
#include "DelegateCallback.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_AwaitSkillSelection::Action_AwaitSkillSelection(SkillType skillType)
    {
        this->skillType = skillType;
    };

    CoroutineObject<void> Action_AwaitSkillSelection::DoAction()
    {
        bool skillSelected = false;
        switch (skillType)
        {
        case SkillType::ROBIN_Q: case SkillType::ROBIN_W:
        case SkillType::URSULA_Q: case SkillType::URSULA_W:
        case SkillType::HANSEL_Q: case SkillType::HANSEL_W:
            PlayerController::Instance().onSkillSelect[(int)(SkillType)skillType].AddVolatileCallback([&skillSelected]() {skillSelected = true; });
            break;
        };
        while (!skillSelected)
        {
            co_await std::suspend_always{};
        }
        co_return;
    };

    void Action_AwaitSkillSelection::ImGui_DrawDataPopup(Action_AwaitSkillSelection* data)
    {
        if (ImGui::MenuItem("SetSkillType"))
        {
            editor::EditorLayer::SetInputControl(false);
            static POD_Enum<SkillType> skillType = SkillType::NONE;
            editor::imgui::ShowMessageBox("SetSkillType", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    imgui::data::DrawData("SkillType", skillType);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->skillType = skillType;;
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

    bool Action_AwaitSkillSelection::PreEncoding(json& data) const
    {
        data["skillType"] = (int)(SkillType)skillType;
        return true;
    }

    bool Action_AwaitSkillSelection::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_AwaitSkillSelection::PreDecoding(const json& data)
    {
        if (data.contains("index"))
        {
            skillType = (SkillType)data["index"].get<int>();
        }
        else
        {
            skillType = (SkillType)data["skillType"].get<int>();
        }
        return true;
    }

    bool Action_AwaitSkillSelection::PostDecoding(const json& data)
    {
        return true;
    }
}