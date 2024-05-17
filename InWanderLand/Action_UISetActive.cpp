#include "Action_UISetActive.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "UIManager.h"
#include "UIElement.h"

#include "CinematicManager.h"

#include "Action_WaitForSeconds.h"

namespace application
{
    CoroutineObject<void> Action_UISetActive::DoAction()
    {
        auto elem{ UIManager::Instance().GetScriptUI(uiName) };
        if (active)
        {
            elem->EnableElement();
        }
        else
        {
            elem->DisableElement();
        }
        co_return;
    }

    void Action_UISetActive::ImGui_DrawDataPopup(Action_UISetActive* data)
    {
        if (ImGui::MenuItem("SetUIKey"))
        {
            editor::EditorLayer::SetInputControl(false);
            static string uiName = "";
            static bool active = true;
            uiName = data->uiName;
            active = data->active;
            editor::imgui::ShowMessageBox("SetUIKey", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    if (UIManager::Instance().GetScriptUI_KeyStrings().empty())
                    {
                        UIManager::Instance().ImportUI("InWanderLand.iwui");
                        UIManager::Instance().Clear();
                    }
                    if (ImGui::BeginCombo("UI Key ID", uiName.c_str()))
                    {
                        for (const auto& key : UIManager::Instance().GetScriptUI_KeyStrings())
                        {
                            bool isSelected = (data->uiName == key);
                            if (ImGui::Selectable(key.c_str(), isSelected))
                            {
                                uiName = key;
                            }
                            if (isSelected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::Checkbox("Active&&set ui active", &active);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->uiName = uiName;
                        data->active = active;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetUIKey");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetUIKey");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_UISetActive::PreEncoding(json& data) const
    {
        data["uiName"] = uiName;
        data["active"] = active;
        return true;
    }

    bool Action_UISetActive::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_UISetActive::PreDecoding(const json& data)
    {
        uiName = data["uiName"];
        active = data["active"];
        return true;
    }

    bool Action_UISetActive::PostDecoding(const json& data)
    {
        return true;
    }
}