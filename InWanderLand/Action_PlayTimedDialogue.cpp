#include "Action_PlayTimedDialogue.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "UIManager.h"
#include "UIElement.h"

#include "CinematicManager.h"

#include "Action_WaitForSeconds.h"

namespace application
{
    CoroutineObject<void> Action_PlayTimedDialogue::DoAction()
    {
        auto elem{ UIManager::Instance().GetDialogueTimed(dialogueKeyID) };
        elem->EnableElement();
        co_return;
    }

    void Action_PlayTimedDialogue::ImGui_DrawDataPopup(Action_PlayTimedDialogue* data)
    {
        if (ImGui::MenuItem("SetDialogueKey"))
        {
            editor::EditorLayer::SetInputControl(false);
            static string dialogueKeyID = "";
            dialogueKeyID = data->dialogueKeyID;
            editor::imgui::ShowMessageBox("SetDialogueKey", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    if (UIManager::Instance().GetDialogueTimed_KeyStrings().empty())
                    {
                        UIManager::Instance().ImportUI("InWanderLand.iwui");
                        UIManager::Instance().Clear();
                    }
                    if (ImGui::BeginCombo("Dialogue Key ID", dialogueKeyID.c_str()))
                    {
                        for (const auto& key : UIManager::Instance().GetDialogueTimed_KeyStrings())
                        {
                            bool isSelected = (data->dialogueKeyID == key);
                            if (ImGui::Selectable(key.c_str(), isSelected))
                            {
                                dialogueKeyID = key;
                            }
                            if (isSelected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->dialogueKeyID = dialogueKeyID;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetDialogueKey");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetDialogueKey");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_PlayTimedDialogue::PreEncoding(json& data) const
    {
        data["dialogueKeyID"] = dialogueKeyID;
        return true;
    }

    bool Action_PlayTimedDialogue::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_PlayTimedDialogue::PreDecoding(const json& data)
    {
        dialogueKeyID = data["dialogueKeyID"];
        return true;
    }

    bool Action_PlayTimedDialogue::PostDecoding(const json& data)
    {
        return true;
    }
}