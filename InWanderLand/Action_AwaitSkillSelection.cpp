#include "Action_AwaitSkillSelection.h"
#include "LocalTimeEntityManager.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_AwaitSkillSelection::Action_AwaitSkillSelection(int skillIndex)
    {
        this->index = skillIndex;
    }

    CoroutineObject<void> Action_AwaitSkillSelection::DoAction()
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
        PlayerController::Instance().skillSelectionCallback[unitType][skillType].push_back([&skillSelected]() { skillSelected = true; });
        while (!skillSelected)
        {
            co_await std::suspend_always{};
        }
        co_return;
    }

    void Action_AwaitSkillSelection::SetSkillIndex(int index)
    {
        this->index = index;
    }

    void Action_AwaitSkillSelection::ImGui_DrawDataPopup(Action_AwaitSkillSelection* data)
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
                    ImGui::InputInt("##AwaitSkillSelectionIndex", &index);

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

    bool Action_AwaitSkillSelection::PreEncoding(json& data) const
    {
        data["index"] = index;
        return true;
    }

    bool Action_AwaitSkillSelection::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_AwaitSkillSelection::PreDecoding(const json& data)
    {
        index = data["index"];
        return true;
    }

    bool Action_AwaitSkillSelection::PostDecoding(const json& data)
    {
        return true;
    }
}