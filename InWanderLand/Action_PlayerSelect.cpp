#include "Action_PlayerSelect.h"
#include "LocalTimeEntityManager.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_PlayerSelect::Action_PlayerSelect(int playerIndex)
    {
        this->index = playerIndex;
    }

    CoroutineObject<void> Action_PlayerSelect::DoAction()
    {
        UIEnumID uiType = UIEnumID::CharInfo_Ursula;
        switch (index)
        {
        case 1:
            uiType = UIEnumID::CharInfo_Robin;
            break;
        case 2:
            uiType = UIEnumID::CharInfo_Ursula;
            break;
        case 3:
            uiType = UIEnumID::CharInfo_Hansel;
            break;
        }
        UIManager::Instance().GetUIElementByEnum(uiType)->
            GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeButtonClickEvent();
        co_return;
    }

    void Action_PlayerSelect::SetSkillIndex(int index)
    {
        this->index = index;
    }

    void Action_PlayerSelect::ImGui_DrawDataPopup(Action_PlayerSelect* data)
    {
        if (ImGui::MenuItem("SetPlayerIndex - 1:Robin 2:Urusla 3:Hansel"))
        {
            editor::EditorLayer::SetInputControl(false);
            static int index = 0;
            index = data->index;
            editor::imgui::ShowMessageBox("SetPlayerIndex - 1:Robin 2:Urusla 3:Hansel", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::InputInt("##PlayerSelectIndex", &index);

                    ImGui::Separator();
                    index = clamp(index, 1, 6);

                    if (ImGui::Button("OK"))
                    {
                        data->SetSkillIndex(index);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetPlayerIndex - 1:Robin 2:Urusla 3:Hansel");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetPlayerIndex - 1:Robin 2:Urusla 3:Hansel");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_PlayerSelect::PreEncoding(json& data) const
    {
        data["index"] = index;
        return true;
    }

    bool Action_PlayerSelect::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_PlayerSelect::PreDecoding(const json& data)
    {
        index = data["index"];
        return true;
    }

    bool Action_PlayerSelect::PostDecoding(const json& data)
    {
        return true;
    }
}