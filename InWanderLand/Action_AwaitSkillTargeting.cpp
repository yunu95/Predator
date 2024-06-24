#include "Action_AwaitSkillTargeting.h"
#include "LocalTimeEntityManager.h"
#include "Skill.h"
#include "DelegateCallback.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_AwaitSkillTargeting::Action_AwaitSkillTargeting(SkillType::Enum skillType)
    {
        this->skillType.enumValue = skillType;
        this->skillType.enumName = POD_Enum<SkillType::Enum>::GetEnumNameMap().at(skillType);
    };

    CoroutineObject<void> Action_AwaitSkillTargeting::DoAction()
    {
        bool proceed = false;
        switch (skillType.enumValue)
        {
        case SkillType::ROBIN_Q: case SkillType::ROBIN_W:
        case SkillType::URSULA_Q: case SkillType::URSULA_W:
        case SkillType::HANSEL_Q: case SkillType::HANSEL_W:
            PlayerController::Instance().onSkillTargeted[skillType.enumValue].AddVolatileCallback([&proceed]() {proceed = true; });
            break;
        };
        while (!proceed)
        {
            co_await std::suspend_always{};
        }
        co_return;
    };

    void Action_AwaitSkillTargeting::ImGui_DrawDataPopup(Action_AwaitSkillTargeting* data)
    {
        if (ImGui::MenuItem("SetSkillType##ForTargeting"))
        {
            editor::EditorLayer::SetInputControl(false);
            static POD_Enum<SkillType::Enum> skillType = POD_Enum<SkillType::Enum>();
            skillType = data->skillType;
            editor::imgui::ShowMessageBox("SetSkillType(AwaitSkillTargeting)", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);

                    if (ImGui::BeginTable("##(AwaitSkillTargeting)", 2, ImGuiTableFlags_SizingStretchSame))
                    {
                        imgui::data::DrawData("SkillType##(AwaitSkillTargeting)", skillType);
                        ImGui::EndTable();
                    }

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->skillType = skillType;;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSkillType(AwaitSkillTargeting)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSkillType(AwaitSkillTargeting)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_AwaitSkillTargeting::PreEncoding(json& data) const
    {
        data["skillType"] = skillType.enumValue;
        return true;
    }

    bool Action_AwaitSkillTargeting::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_AwaitSkillTargeting::PreDecoding(const json& data)
    {
        skillType.enumValue = data["skillType"];
        skillType.enumName = POD_Enum<SkillType::Enum>::GetEnumNameMap().at(skillType.enumValue);
        return true;
    }

    bool Action_AwaitSkillTargeting::PostDecoding(const json& data)
    {
        return true;
    }
}