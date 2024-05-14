#include "Action_PlayManualDialogue.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "UIManager.h"
#include "UIElement.h"

#include "CinematicManager.h"

#include "Action_WaitForSeconds.h"

namespace application
{
    CoroutineObject<void> Action_PlayManualDialogue::DoAction()
    {
        auto elem{ UIManager::Instance().GetDialogueManual(dialogueKeyID) };
        elem->EnableElement();
        while (elem->GetGameObject()->GetActive()) co_await std::suspend_always();
        co_return;
    }

    void Action_PlayManualDialogue::ImGui_DrawDataPopup(Action_PlayManualDialogue* data)
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
                    if (UIManager::Instance().GetDialogueManual_KeyStrings().empty())
                    {
                        UIManager::Instance().ImportUI("InWanderLand.iwui");
                        UIManager::Instance().Clear();
                    }
                    if (ImGui::BeginCombo("Dialogue Key ID", dialogueKeyID.c_str()))
                    {
                        for (const auto& key : UIManager::Instance().GetDialogueManual_KeyStrings())
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

    bool Action_PlayManualDialogue::PreEncoding(json& data) const
    {
        data["dialogueKeyID"] = dialogueKeyID;
        return true;
    }

    bool Action_PlayManualDialogue::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_PlayManualDialogue::PreDecoding(const json& data)
    {
        dialogueKeyID = data["dialogueKeyID"];
        return true;
    }

    bool Action_PlayManualDialogue::PostDecoding(const json& data)
    {
        return true;
    }
}