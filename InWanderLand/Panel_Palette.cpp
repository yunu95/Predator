#include "InWanderLand.h"
#include "Panel_Palette.h"
#include "Application.h"
#include "imgui_Utility.h"

#include "YunutyEngine.h"

#include "imgui.h"

namespace application
{
    namespace editor
    {
        PalettePanel::~PalettePanel()
        {

        }

        void PalettePanel::Initialize()
        {
            currentPalette = pm.GetCurrentPalette();
        }

        void PalettePanel::Update(float ts)
        {

        }

        void PalettePanel::GUIProgress()
        {
            ImGui::Begin("Palette");

            ImGui_Update();

            /// 실제 패널에 그리는 영역
            if (ImGui::BeginTabBar("PaletteTabBar", ImGuiTabBarFlags_None))
            {
                /// 플래그를 사용하여 정렬 상태를 조정해야 하지만,
                /// 그냥 간편하게 기본적으로 Terrain 으로 시작된다고 가정함
                if (ImGui::BeginTabItem("Terrain"))
                {
                    ChangePalette(&tp);
                    ImGui_BeginTerrainPalette();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Unit"))
                {
                    ChangePalette(&up);
                    ImGui_BeginUnitPalette();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Doodad"))
                {
                    ChangePalette(&dp);
                    ImGui_BeginDoodadPalette();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Region"))
                {
                    ChangePalette(&rp);
                    ImGui_BeginRegionPalette();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::End();
        }

        void PalettePanel::Finalize()
        {

        }

        PalettePanel::PalettePanel()
        {

        }

        void PalettePanel::ChangePalette(palette::Palette* palette)
        {
            if (currentPalette != palette)
            {
                pm.SetCurrentPalette(palette);
                currentPalette = palette;
            }
        }

        void PalettePanel::ImGui_Update()
        {
            /// ImGui 관련 내부 변수 업데이트
            isMouseOver = ImGui::IsWindowHovered();
            isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

            if (!Application::IsFocusGameWindow())
            {
                // 마우스 입력에 대한 처리
                if (isMouseOver)
                {

                }

                // 키 입력에 대한 처리
                if (eim.IsKeyboardUp(KeyCode::ESC))
                {
                    if (currentPalette && !currentPalette->IsSelectMode())
                    {
                        currentPalette->SetAsSelectMode(true);
                    }
                }

                if (eim.IsKeyboardUp(KeyCode::Delete))
                {
                    if (currentPalette && currentPalette->IsSelectMode())
                    {
                        currentPalette->OnDeletion();
                    }
                }
            }
        }

        void PalettePanel::ImGui_BeginTerrainPalette()
        {
            imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

            if (ImGui::BeginTable("TerrainPaletteTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip))
            {
                auto modeButtonSize = ImGui::CalcTextSize("Mode");
                modeButtonSize.x += ImGui::GetStyle().WindowPadding.x + 4.0f;
                ImGui::TableSetupColumn("label_column", 0, modeButtonSize.x);
                ImGui::TableSetupColumn("value_column", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip, ImGui::GetContentRegionAvail().x - modeButtonSize.x);

                ImGui::TableNextRow();
                {
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Button("Mode"))
                    {
                        if (currentPalette)
                        {
                            currentPalette->SetAsSelectMode(!currentPalette->IsSelectMode());
                        }
                    }

                    ImGui::TableSetColumnIndex(1);
                    if (currentPalette->IsSelectMode())
                    {
                        ImGui::Text("Select");
                    }
                    else
                    {
                        ImGui::Text("Place");
                    }
                }

                ImGui::EndTable();
            }
        }

        void PalettePanel::ImGui_BeginUnitPalette()
        {
            imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

            if (ImGui::BeginTable("UnitPaletteTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip))
            {
                ImGui::TableSetupColumn("label_column", 0, 100);
                ImGui::TableSetupColumn("value_column", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip, ImGui::GetContentRegionAvail().x);

                ImGui::TableNextRow();
                {
                    ImGui::TableSetColumnIndex(0);

                    ImGui::TableSetColumnIndex(1);
                }

                ImGui::EndTable();
            }
        }

        void PalettePanel::ImGui_BeginDoodadPalette()
        {
            imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

            if (ImGui::BeginTable("DoodadPaletteTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip))
            {
                ImGui::TableSetupColumn("label_column", 0, 100);
                ImGui::TableSetupColumn("value_column", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip, ImGui::GetContentRegionAvail().x);

                ImGui::TableNextRow();
                {
                    ImGui::TableSetColumnIndex(0);

                    ImGui::TableSetColumnIndex(1);
                }

                ImGui::EndTable();
            }
        }

        void PalettePanel::ImGui_BeginRegionPalette()
        {
            imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

            if (ImGui::BeginTable("DoodadPaletteTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip))
            {
                ImGui::TableSetupColumn("label_column", 0, 100);
                ImGui::TableSetupColumn("value_column", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip, ImGui::GetContentRegionAvail().x);

                ImGui::TableNextRow();
                {
                    ImGui::TableSetColumnIndex(0);

                    ImGui::TableSetColumnIndex(1);
                }

                ImGui::EndTable();
            }
        }
    }
}
