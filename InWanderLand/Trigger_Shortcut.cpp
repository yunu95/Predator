#include "Trigger_Shortcut.h"
#include "ShortcutSystem.h"

#include "GamePlayTimer.h"
#include "EditorLayer.h"

namespace application
{
    void Trigger_Shortcut::LinkCallback()
    {
        std::vector<std::pair<KeyCode, bool>> keys;
        switch (shortcutType.enumValue)
        {
        case ShortcutType::Enum::NONE: return;
        case ShortcutType::Enum::CTRL_ALT_1: keys = { { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::NUM_1, false } }; break;
        case ShortcutType::Enum::CTRL_ALT_2: keys = { { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::NUM_2, false } }; break;
        case ShortcutType::Enum::CTRL_ALT_3: keys = { { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::NUM_3, false } }; break;
        case ShortcutType::Enum::CTRL_ALT_4: keys = { { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::NUM_4, false } }; break;
        case ShortcutType::Enum::CTRL_ALT_5: keys = { { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::NUM_5, false } }; break;
        case ShortcutType::Enum::CTRL_ALT_6: keys = { { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::NUM_6, false } }; break;
        case ShortcutType::Enum::CTRL_ALT_7: keys = { { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::NUM_7, false } }; break;
        case ShortcutType::Enum::CTRL_ALT_8: keys = { { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::NUM_8, false } }; break;
        case ShortcutType::Enum::CTRL_ALT_9: keys = { { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::NUM_9, false } }; break;
        case ShortcutType::Enum::CTRL_ALT_0: keys = { { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::NUM_0, false } }; break;
        }
        auto& scsys = ShortcutSystem::Instance();
        auto keyIdx = scsys.GetKeyIndex(keys);
        if (keyIdx.IsValid())
        {
            scsys.RemoveUniqueTrigger(keyIdx);
        }
        scsys.RegisterUniqueTrigger(keys, [=]() { PullTrigger(); });
    }

    void Trigger_Shortcut::ImGui_DrawDataPopup(Trigger_Shortcut* data)
    {
        if (ImGui::MenuItem("SetShortcutType"))
        {
            editor::EditorLayer::SetInputControl(false);
            static POD_Enum<ShortcutType::Enum> shortcutType = POD_Enum<ShortcutType::Enum>();
            shortcutType = data->shortcutType;
            editor::imgui::ShowMessageBox("SetShortcutType", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    if (ImGui::BeginTable("##SetShortcut", 2, ImGuiTableFlags_SizingStretchSame))
                    {
                        imgui::data::DrawData("ShortcutType", shortcutType);
                        ImGui::EndTable();
                    }

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->shortcutType = shortcutType;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetShortcutType");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetShortcutType");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Trigger_Shortcut::PreEncoding(json& data) const
    {
        data["shortcutType"] = shortcutType.enumValue;
        return true;
    }

    bool Trigger_Shortcut::PostEncoding(json& data) const
    {
        return true;
    }

    bool Trigger_Shortcut::PreDecoding(const json& data)
    {
        shortcutType.enumValue = data["shortcutType"];
        shortcutType.enumName = POD_Enum<ShortcutType::Enum>::GetEnumNameMap().at(shortcutType.enumValue);
        return true;
    }

    bool Trigger_Shortcut::PostDecoding(const json& data)
    {
        return true;
    }
}