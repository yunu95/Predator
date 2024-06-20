#include "Action_AwaitSkillActivation.h"
#include "LocalTimeEntityManager.h"
#include "Skill.h"
#include "DelegateCallback.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_AwaitSkillActivation::Action_AwaitSkillActivation(SkillType::Enum skillType)
    {
        this->skillType.enumValue = skillType;
        this->skillType.enumName = POD_Enum<SkillType::Enum>::GetEnumNameMap().at(skillType);
    };

    CoroutineObject<void> Action_AwaitSkillActivation::DoAction()
    {
        bool skillSelected = false;
        switch (skillType.enumValue)
        {
        case SkillType::ROBIN_Q: case SkillType::ROBIN_W:
        case SkillType::URSULA_Q: case SkillType::URSULA_W:
        case SkillType::HANSEL_Q: case SkillType::HANSEL_W:
            PlayerController::Instance().onSkillActivate[skillType.enumValue].AddVolatileCallback([&skillSelected]() {skillSelected = true; });
            break;
        };
        while (!skillSelected)
        {
            co_await std::suspend_always{};
        }
        co_return;
    };

    void Action_AwaitSkillActivation::ImGui_DrawDataPopup(Action_AwaitSkillActivation* data)
    {
        if (ImGui::MenuItem("SetSkillType"))
        {
            editor::EditorLayer::SetInputControl(false);
            static POD_Enum<SkillType::Enum> skillType = POD_Enum<SkillType::Enum>();
            skillType = data->skillType;
            editor::imgui::ShowMessageBox("SetSkillType(AwaitSkillActivation)", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);

                    if (ImGui::BeginTable("##BlockSkillSelection", 2, ImGuiTableFlags_SizingStretchSame))
                    {
                        imgui::data::DrawData("SkillType", skillType);
                        ImGui::EndTable();
                    }

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->skillType = skillType;;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSkillType(AwaitSkillActivation)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSkillType(AwaitSkillActivation)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_AwaitSkillActivation::PreEncoding(json& data) const
    {
        data["skillType"] = skillType.enumValue;
        return true;
    }

    bool Action_AwaitSkillActivation::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_AwaitSkillActivation::PreDecoding(const json& data)
    {
        skillType.enumValue = data["skillType"];
        skillType.enumName = POD_Enum<SkillType::Enum>::GetEnumNameMap().at(skillType.enumValue);
        return true;
    }

    bool Action_AwaitSkillActivation::PostDecoding(const json& data)
    {
        return true;
    }
}