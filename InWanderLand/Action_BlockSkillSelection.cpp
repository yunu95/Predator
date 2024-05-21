#include "Action_BlockSkillSelection.h"
#include "LocalTimeEntityManager.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_BlockSkillSelection::Action_BlockSkillSelection(int skillIndex)
    {
        this->index = skillIndex;
    }

    CoroutineObject<void> Action_BlockSkillSelection::DoAction()
    {
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
        if (applyExceptTarget)
        {
            bool before = InputManager::Instance().canPrepareSkill[unitType][skillType];

            InputManager::Instance().canPrepareSkill[Unit::UnitType::Warrior][Unit::SkillEnum::Q] = !blocking;
            InputManager::Instance().canPrepareSkill[Unit::UnitType::Warrior][Unit::SkillEnum::W] = !blocking;
            InputManager::Instance().canPrepareSkill[Unit::UnitType::Magician][Unit::SkillEnum::Q] = !blocking;
            InputManager::Instance().canPrepareSkill[Unit::UnitType::Magician][Unit::SkillEnum::W] = !blocking;
            InputManager::Instance().canPrepareSkill[Unit::UnitType::Healer][Unit::SkillEnum::Q] = !blocking;
            InputManager::Instance().canPrepareSkill[Unit::UnitType::Healer][Unit::SkillEnum::W] = !blocking;

            InputManager::Instance().canPrepareSkill[unitType][skillType] = before;
        }
        else
        {
            InputManager::Instance().canPrepareSkill[unitType][skillType] = !blocking;
        }
        co_return;
    }

    void Action_BlockSkillSelection::SetSkillIndex(int index)
    {
        this->index = index;
    }

    void Action_BlockSkillSelection::ImGui_DrawDataPopup(Action_BlockSkillSelection* data)
    {
        if (ImGui::MenuItem("SetSkillIndex - 1:RobinQ 2:RobinW 3:UruslaQ 4:UruslaW 5:HanselQ 6::HanselW"))
        {
            editor::EditorLayer::SetInputControl(false);
            static int index = 0;
            static bool blocking{ true };
            static bool applyExceptTarget{ false };
            index = data->index;
            blocking = data->blocking;
            applyExceptTarget = data->applyExceptTarget;

            editor::imgui::ShowMessageBox("SetSkillIndex - 1:RobinQ 2:RobinW 3:UruslaQ 4:UruslaW 5:HanselQ 6::HanselW", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("ApplyExceptTargetSkill##ApplyExceptTarget", &applyExceptTarget);
                    ImGui::InputInt("SkillIndex##BlockSkillSelectionIndex", &index);
                    ImGui::Checkbox("Blocking##Blocking", &blocking);

                    ImGui::Separator();
                    index = clamp(index, 1, 6);

                    if (ImGui::Button("OK"))
                    {
                        data->SetSkillIndex(index);
                        data->blocking = blocking;
                        data->applyExceptTarget = applyExceptTarget;
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

    bool Action_BlockSkillSelection::PreEncoding(json& data) const
    {
        data["blocking"] = blocking;
        data["applyExceptTarget"] = applyExceptTarget;
        data["index"] = index;
        return true;
    }

    bool Action_BlockSkillSelection::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_BlockSkillSelection::PreDecoding(const json& data)
    {
        index = data["index"];
        blocking = data["blocking"];
        applyExceptTarget = data["applyExceptTarget"];
        return true;
    }

    bool Action_BlockSkillSelection::PostDecoding(const json& data)
    {
        return true;
    }
}