#include "Action_AwaitSkillActivation.h"
#include "LocalTimeEntityManager.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_AwaitSkillActivation::Action_AwaitSkillActivation(int skillIndex)
    {
        this->index = skillIndex;
    }

    CoroutineObject<void> Action_AwaitSkillActivation::DoAction()
    {
        bool skillSelected = false;
        Unit::UnitType unitType = Unit::UnitType::Warrior;
        Unit::SkillEnum skillType;
        switch (index)
        {
        case 1: case 2:
            unitType = Unit::UnitType::Warrior;
            break;
        case 3: case 4:
            unitType = Unit::UnitType::Magician;
            break;
        case 5: case 6:
            unitType = Unit::UnitType::Healer;
            break;
        };
        switch (index)
        {
        case 1: case 3:case 5:
            skillType = Unit::SkillEnum::Q;
            break;
        case 2:case 4: case 6:
            skillType = Unit::SkillEnum::W;
            break;
        };
        PlayerController::Instance().skillActivationCallback[unitType][skillType].push_back([&skillSelected]() { skillSelected = true; });
        while (!skillSelected)
        {
            co_await std::suspend_always{};
        }
        co_return;
    }

    void Action_AwaitSkillActivation::SetSkillIndex(int index)
    {
        this->index = index;
    }

    void Action_AwaitSkillActivation::ImGui_DrawDataPopup(Action_AwaitSkillActivation* data)
    {
        if (ImGui::MenuItem("SetSkillIndex - 1:RobinQ 2:RobinW 3:UruslaQ 4:UruslaW 5:HanselQ 6::HanselW"))
        {
            editor::EditorLayer::SetInputControl(false);
            static int index = 0;
            index = data->index;
            editor::imgui::ShowMessageBox("SetSkillIndex - 1:RobinQ 2:RobinW 3:UruslaQ 4:UruslaW 5:HanselQ 6::HanselW", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::InputInt("##AwaitSkillActivationIndex", &index);

                    ImGui::Separator();
                    index = clamp(index, 1, 6);

                    if (ImGui::Button("OK"))
                    {
                        data->SetSkillIndex(index);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSkillIndex - 1:RobinQ 2:RobinW 3:UruslaQ 4:UruslaW 5:HanselQ 6::HanselW");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSkillIndex - 1:RobinQ 2:RobinW 3:UruslaQ 4:UruslaW 5:HanselQ 6::HanselW");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_AwaitSkillActivation::PreEncoding(json& data) const
    {
        data["index"] = index;
        return true;
    }

    bool Action_AwaitSkillActivation::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_AwaitSkillActivation::PreDecoding(const json& data)
    {
        index = data["index"];
        return true;
    }

    bool Action_AwaitSkillActivation::PostDecoding(const json& data)
    {
        return true;
    }
}