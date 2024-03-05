#include "InWanderLand.h"
#include "Panel_Palette.h"
#include "Application.h"
#include "imgui_Utility.h"
#include "EditorCommonEvents.h"
#include "Wave_TemplateData.h"
#include "WaveData.h"
#include "Region_TemplateData.h"
#include "RegionData.h"
#include "SpecialEvent.h"
#include "WavePalette.h"
#include "RegionPalette.h"

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

            auto uSize = tdm.GetDataList(DataType::UnitData).size();
            unitButton.reserve(30);
            for (int i = 0; i < uSize; i++)
            {
                unitButton.push_back(false);
            }
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
                bool tabJustOpened = false;
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

                if (ImGui::BeginTabItem("Ornament"))
                {
                    ChangePalette(&op);
                    ImGui_BeginOrnamentPalette();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Region"))
                {
                    ChangePalette(&rp);
                    ImGui_BeginRegionPalette();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Wave"))
                {
                    ChangePalette(&wp);
                    ImGui_BeginWavePalette();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::End();
        }

        void PalettePanel::Finalize()
        {

        }

        void PalettePanel::OnEvent(EditorEvents& event)
        {
            EventDispatcher dispatcher(event);
            dispatcher.Dispatch<LoadEvent>([this](LoadEvent& e)
                {
                    LoadCallback();
                    return true;
                });
        }

        PalettePanel::PalettePanel()
        {
            unitButton.reserve(20);
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

                        if (currentPalette == &up)
                        {
                            if (unitCurrentButton != -1)
                            {
                                unitButton[unitCurrentButton] = false;
                                unitCurrentButton = -1;
                            }

                            up.SelectUnitTemplateData(nullptr);
                        }
                        else if (currentPalette == &op)
                        {
                            ornamentCurrentButton = -1;
                            op.SelectOrnamentTemplateData(nullptr);
                        }
                    }
                }

                if (eim.IsKeyboardUp(KeyCode::Delete))
                {
                    if (currentPalette && currentPalette->IsSelectMode())
                    {
                        currentPalette->OnDeletion();
                    }
                }

                if (eim.IsKeyboardUp(KeyCode::B))
                {
                    if (currentPalette == &tp)
                    {
                        tp.ApplyAsPlaytimeObjects();
                    }
                }
            }
        }

        void PalettePanel::ImGui_BeginTerrainPalette()
        {
            imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

            int countIdx = 0;

            if (imgui::BeginSection_1Col(countIdx, "Place Mode", ImGui::GetContentRegionAvail().x))
            {
                bool pButton = !tp.IsSelectMode() && tp.IsMarking();
                bool eButton = !tp.IsSelectMode() && !tp.IsMarking();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                bool pFlag = imgui::SelectableImageButton("Terrain_PlaceButton", "ImageButtons/Terrain_PlaceButton.png", pButton, ImVec2(50, 50));
                ImGui::SameLine();
                bool eFlag = imgui::SelectableImageButton("Terrain_EraseButton", "ImageButtons/Terrain_EraseButton.png", eButton, ImVec2(50, 50));

                if (pFlag)
                {
                    if (pButton)
                    {
                        tp.SetAsSelectMode(true);
                    }
                    else
                    {
                        tp.SetAsSelectMode(false);
                        tp.SetIsMarking(true);
                    }
                }

                if (eFlag)
                {
                    if (eButton)
                    {
                        tp.SetAsSelectMode(true);
                    }
                    else
                    {
                        tp.SetAsSelectMode(false);
                        tp.SetIsMarking(false);
                    }
                }

                imgui::EndSection();
            }

            if (imgui::BeginSection_2Col(countIdx, "Brush Options", ImGui::GetContentRegionAvail().x, 0.3f))
            {
                int bSize = tp.GetBrushSize();

                if (imgui::SliderInt_2Col("Size", bSize, 0, 15))
                {
                    tp.SetBrushSize(bSize);
                }

                imgui::EndSection();
            }

            if (imgui::BeginSection_1Col(countIdx, "Navigation Build", ImGui::GetContentRegionAvail().x))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Button("Build", ImVec2(ImGui::GetContentRegionAvail().x, 50)))
                {
                    tp.ApplyAsPlaytimeObjects();
                }

                imgui::EndSection();
            }
        }

        void PalettePanel::ImGui_BeginUnitPalette()
        {
            imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

            int countIdx = 0;
            bool isPlacingWaveUnit = palette::WavePalette::SingleInstance().currentWaveData && palette::WavePalette::SingleInstance().currentSelectedWaveIndex >= 0;
            if (isPlacingWaveUnit)
            {
                //stringstream ss;
                //ss << "Wave is selected. Please select time offset and unit type to place.";
                //ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.04f, 0.0f, 0.96f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
                //ImGui::TextColored({ 1,0,0,1 }, "Wave is selected. Please select time offset and unit type to place.");
                ImGui::TextWrapped("Wave is selected. Please select time offset and unit type to place.");
                ImGui::PopStyleColor();
                ImGui::DragFloat("time offset", &palette::WavePalette::SingleInstance().currentSelectedWaveTimeOffset, 0.05, 0, 10000);
            }
            if (imgui::BeginSection_1Col(countIdx, "Unit List", ImGui::GetContentRegionAvail().x))
            {
                auto uSize = tdm.GetDataList(DataType::UnitData).size();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                int colCount = 6;
                int rowCount = 1;

                auto style = ImGui::GetStyle();
                auto imageSize = ImGui::GetContentRegionAvail().x / colCount - style.ItemSpacing.x - style.FramePadding.x * 2;

                for (int i = 0; i < uSize + 1; i++)
                {
                    if (i != 0 && i % colCount == 0)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                    }

                    if (i == uSize)
                    {
                        bool buttonFlag = imgui::SelectableImageButton("Unit Add Button", "ImageButtons/Unit_AddButton.png", false, ImVec2(imageSize, imageSize));
                        imgui::SetTooltip("Add");
                        if (buttonFlag)
                        {
                            if (unitCurrentButton != -1)
                            {
                                unitButton[unitCurrentButton] = false;
                                unitCurrentButton = -1;
                                up.SelectUnitTemplateData(nullptr);
                                up.SetAsSelectMode(true);
                            }

                            // Unit Template 추가 로직과 연결해야 함
                            // 이때, unitButton 에도 pushback 해주어 Size를 추가해야 함
                            /// 임시로 Unit Template Data 하나를 추가하는 로직을 구현함
                            auto td = tdm.CreateTemplateData<Unit_TemplateData>("UnitButton" + std::to_string(i));
                            td->SetDataResourceName("SM_Bush_001");
                            unitButton.push_back(false);
                        }
                    }
                    else
                    {
                        bool ref = unitButton[i];
                        bool buttonFlag = imgui::SelectableImageButton("UnitButton" + std::to_string(i), static_cast<Unit_TemplateData*>(tdm.GetDataList(DataType::UnitData)[i])->pod.thumbnailPath, ref, ImVec2(imageSize, imageSize));
                        imgui::SetTooltip(tdm.GetDataList(DataType::UnitData)[i]->GetDataKey());
                        if (buttonFlag)
                        {
                            if (unitCurrentButton != -1)
                            {
                                unitButton[unitCurrentButton] = false;

                                if (unitCurrentButton == i)
                                {
                                    unitCurrentButton = -1;
                                    up.SelectUnitTemplateData(nullptr);
                                    up.SetAsSelectMode(true);
                                }
                                else
                                {
                                    unitButton[i] = true;
                                    unitCurrentButton = i;
                                    up.SelectUnitTemplateData(static_cast<Unit_TemplateData*>(tdm.GetDataList(DataType::UnitData)[i]));
                                    up.SetAsSelectMode(false);
                                }
                            }
                            else
                            {
                                unitButton[i] = true;
                                unitCurrentButton = i;
                                up.SelectUnitTemplateData(static_cast<Unit_TemplateData*>(tdm.GetDataList(DataType::UnitData)[i]));
                                up.SetAsSelectMode(false);
                            }
                        }

                        ImGui::SameLine();
                    }
                }
                imgui::EndSection();
            }
        }

        void PalettePanel::ImGui_BeginOrnamentPalette()
        {
            imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

            int countIdx = 0;

            if (imgui::BeginSection_1Col(countIdx, "Ornament List", ImGui::GetContentRegionAvail().x))
            {
                auto oSize = tdm.GetDataList(DataType::OrnamentData).size();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                int colCount = 6;
                int rowCount = 1;

                auto style = ImGui::GetStyle();
                auto imageSize = ImGui::GetContentRegionAvail().x / colCount - style.ItemSpacing.x - style.FramePadding.x * 2;

                // 반복 영역을 oSize + 1 로 설정하면 + 버튼 구현됨
                for (int i = 0; i < oSize; i++)
                {
                    // oSize 는 + 버튼으로 구현 도중 중단함
                    if (i == oSize)
                    {
                        imgui::SmartStyleVar textAlign(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
                        imgui::SmartStyleVar border(ImGuiStyleVar_FrameBorderSize, 2);
                        imgui::SmartStyleColor borderCol(ImGuiCol_Border, ImVec4(0.26f, 0.59f, 0.98f, 1.00f));
                        if (ImGui::Selectable("+", false))
                        {
                            if (ornamentCurrentButton != -1)
                            {
                                ornamentCurrentButton = -1;
                                op.SelectOrnamentTemplateData(nullptr);
                                op.SetAsSelectMode(true);
                            }

                            // Ornament Template 추가 로직과 연결해야 함
                            // 이때, ornamentButton 에도 pushback 해주어 Size를 추가해야 함
                            /// 임시로 Ornament Template Data 하나를 추가하는 로직을 구현함
                            auto td = tdm.CreateTemplateData<Ornament_TemplateData>("OrnamentButton" + std::to_string(i));
                            td->SetDataResourceName("SM_Temple_Books");
                        }
                        ImGui::RenderFrameBorder(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                    }
                    else
                    {
                        imgui::SmartStyleVar textAlign(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
                        if (ImGui::Selectable(static_cast<Ornament_TemplateData*>(tdm.GetDataList(DataType::OrnamentData)[i])->GetDataKey().c_str(), ornamentCurrentButton == i))
                        {
                            if (ornamentCurrentButton != -1)
                            {
                                if (ornamentCurrentButton == i)
                                {
                                    ornamentCurrentButton = -1;
                                    op.SelectOrnamentTemplateData(nullptr);
                                    op.SetAsSelectMode(true);
                                }
                                else
                                {
                                    ornamentCurrentButton = i;
                                    op.SelectOrnamentTemplateData(static_cast<Ornament_TemplateData*>(tdm.GetDataList(DataType::OrnamentData)[i]));
                                    op.SetAsSelectMode(false);
                                }
                            }
                            else
                            {
                                ornamentCurrentButton = i;
                                op.SelectOrnamentTemplateData(static_cast<Ornament_TemplateData*>(tdm.GetDataList(DataType::OrnamentData)[i]));
                                op.SetAsSelectMode(false);
                            }
                        }
                    }
                }
                imgui::EndSection();
            }
        }

        void PalettePanel::ImGui_BeginRegionPalette()
        {
            imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

            //static int selectedRegionIdx = 0;
            //RegionData* selectedRegion = nullptr;
            if (ImGui::Button("Make new region"))
            {
                auto newInstance = InstanceManager::GetSingletonInstance().CreateInstance<RegionData>(Region_TemplateData::GetInstance().GetDataKey());
                auto createPoint = graphics::Camera::GetMainCamera()->GetProjectedPoint({ 0.0,0.0 }, graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition().Magnitude(), { 0,1,0 });
                newInstance->pod.angle = 0;
                newInstance->pod.width = 5;
                newInstance->pod.height = 5;
                newInstance->pod.x = createPoint.x;
                newInstance->pod.z = createPoint.z;
                newInstance->ApplyAsPaletteInstance();
            }

            int countIdx{ 0 };
            if (imgui::BeginSection_1Col(countIdx, "Region List", ImGui::GetContentRegionAvail().x))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x * 0.8f,400 }))
                {
                    int countIdx = 0;
                    for (auto each : RegionData::GetInstances())
                    {
                        stringstream ss;
                        ss << yutility::GetString(each->pod.name).c_str() << " ##RegionSelectable" << countIdx;
                        if (ImGui::Selectable(ss.str().c_str(), palette::RegionPalette::SingleInstance().GetSingleSelectedRegion() == each))
                        {
                            palette::RegionPalette::SingleInstance().SelectRegion(each);
                        }
                        if (palette::RegionPalette::SingleInstance().GetSingleSelectedRegion() == each)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                        countIdx++;
                    }
                    ImGui::EndListBox();
                }
                imgui::EndSection();
            }
            if (palette::RegionPalette::SingleInstance().GetSingleSelectedRegion())
            {
                if (imgui::BeginSection_2Col(countIdx, "Region info", ImGui::GetContentRegionAvail().x))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Region name");
                    ImGui::TableNextColumn();
                    string charBuffer = yutility::GetString(palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.name);
                    charBuffer.reserve(64);
                    ImGui::InputText("##RegionNameInputText", &charBuffer[0], 64);
                    palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.name = yutility::GetWString(charBuffer);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("width");
                    ImGui::TableNextColumn();
                    if (ImGui::DragFloat("##RegionWidthInputText", &palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.width, 0.005f, 0.0f, FLT_MAX, "%.2f", 0))
                        palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->ApplyAsPaletteInstance();
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("height");
                    ImGui::TableNextColumn();
                    if (ImGui::DragFloat("##RegionHeightInputText", &palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.height, 0.005f, 0.0f, FLT_MAX, "%.2f", 0))
                        palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->ApplyAsPaletteInstance();
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("angle");
                    ImGui::TableNextColumn();
                    if (ImGui::DragFloat("##RegionAngleInputText", &palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.angle, 0.05f, 0.0f, 360, "%.2f", 0))
                        palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->ApplyAsPaletteInstance();
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("special event");
                    ImGui::TableNextColumn();
                    if (ImGui::BeginCombo("##RegionSpecialEvent", SpecialEventTypeToString(static_cast<SpecialEventType>(palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.specialEvent)).c_str()))
                    {
                        for (int n = 0; n < SpecialEventTypes().size(); n++)
                        {
                            const bool is_selected = (palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.specialEvent == n);
                            if (ImGui::Selectable((SpecialEventTypeStrings()[n] + "##SpecialEventSelectable").c_str(), is_selected))
                                palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.specialEvent = n;

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("is obstacle?");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##RegionIsObstacle", &palette::RegionPalette::SingleInstance().GetSingleSelectedRegion()->pod.isObstacle);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Button("Delete Region"))
                    {
                        palette::RegionPalette::SingleInstance().Delete(palette::RegionPalette::SingleInstance().GetSingleSelectedRegion());
                    }
                    imgui::EndSection();
                }
            }
        }
        void PalettePanel::ImGui_BeginWavePalette()
        {
            imgui::SmartStyleVar spacing(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
            WaveData*& selectedWave = palette::WavePalette::SingleInstance().currentWaveData;
            if (ImGui::Button("Make new wave"))
                InstanceManager::GetSingletonInstance().CreateInstance<WaveData>(Wave_TemplateData::GetInstance().GetDataKey());

            int countIdx{ 0 };
            if (imgui::BeginSection_1Col(countIdx, "Wave List", ImGui::GetContentRegionAvail().x))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x * 0.8f,400 }))
                {
                    int countIdx{ 0 };
                    for (auto each : WaveData::GetInstances())
                    {
                        countIdx++;
                        stringstream ss;
                        ss << yutility::GetString(each->pod.name).c_str() << " ##WaveSelectable" << countIdx;
                        if (ImGui::Selectable(ss.str().c_str(), selectedWave == each))
                        {
                            selectedWave = each;
                        }
                        if (selectedWave == each)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndListBox();
                }
                int countIdx = 0;
                imgui::EndSection();
            }
            if (selectedWave)
            {
                if (imgui::BeginSection_1Col(countIdx, "Wave info", ImGui::GetContentRegionAvail().x))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Wave name");
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    string charBuffer = yutility::GetString(selectedWave->pod.name);
                    charBuffer.reserve(64);
                    ImGui::InputText("##RegionNameInputText", &charBuffer[0], 64);
                    selectedWave->pod.name = yutility::GetWString(charBuffer);

                    imgui::EndSection();
                }
                if (imgui::BeginSection_1Col(countIdx, "Region info", ImGui::GetContentRegionAvail().x))
                {
                    auto triggerRegion{ selectedWave->pod.triggerRegion };
                    auto constraintRegion{ selectedWave->pod.contraintRegion };

                    /// 접근시 웨이브를 촉발시키는 지역을 지정하는 부분
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Triggering region");
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (!triggerRegion)
                        ImGui::BeginDisabled();
                    // 지역 이름을 클릭하면 카메라가 지역의 위치로 이동
                    if (ImGui::Button(triggerRegion ? yutility::GetString(triggerRegion->pod.name.c_str()).c_str() : "Region not selected"));
                    if (!triggerRegion)
                        ImGui::EndDisabled();
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    auto currentPaletteState = palette::WavePalette::SingleInstance().GetWavePaletteState();
                    if (currentPaletteState == palette::WavePalette::WavePaletteState::SelectingTriggerRegion)
                        ImGui::BeginDisabled();
                    if (ImGui::Button("Select region##Trigger"))
                        palette::WavePalette::SingleInstance().SetWavePaletteState(palette::WavePalette::WavePaletteState::SelectingTriggerRegion);
                    if (currentPaletteState == palette::WavePalette::WavePaletteState::SelectingTriggerRegion)
                        ImGui::EndDisabled();

                    /// 웨이브 진행 중 카메라의 이동을 제한하는 지역을 지정하는 부분
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Constraint region");
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (!constraintRegion)
                        ImGui::BeginDisabled();
                    // 지역 이름을 클릭하면 카메라가 지역의 위치로 이동
                    if (ImGui::Button(constraintRegion ? yutility::GetString(constraintRegion->pod.name.c_str()).c_str() : "Region not selected"));
                    if (!constraintRegion)
                        ImGui::EndDisabled();

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    if (currentPaletteState == palette::WavePalette::WavePaletteState::SelectingConstraintRegion)
                        ImGui::BeginDisabled();
                    if (ImGui::Button("Select region##Constraint"))
                        palette::WavePalette::SingleInstance().SetWavePaletteState(palette::WavePalette::WavePaletteState::SelectingConstraintRegion);
                    if (currentPaletteState == palette::WavePalette::WavePaletteState::SelectingConstraintRegion)
                        ImGui::EndDisabled();

                    imgui::EndSection();
                }
                if (imgui::BeginSection_2Col(countIdx, "each wave info", ImGui::GetContentRegionAvail().x))
                {
                    for (int i = 1; i <= selectedWave->pod.waveSizes.size(); i++)
                    {
                        stringstream ss;
                        ss << "Edit wave " << i;
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        // n차 웨이브를 수정하는 부분
                        if (i - 1 == palette::WavePalette::SingleInstance().currentSelectedWaveIndex)
                            ImGui::BeginDisabled();
                        if (ImGui::Button(ss.str().c_str()))
                        {
                            palette::WavePalette::SingleInstance().currentSelectedWaveIndex = i - 1;
                        }
                        else
                        {
                            if (i - 1 == palette::WavePalette::SingleInstance().currentSelectedWaveIndex)
                                ImGui::EndDisabled();
                        }
                        ImGui::TableNextColumn();
                        // n차 웨이브를 삭제하는 부분
                        if (ImGui::Button("X"));
                    }
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    stringstream ss;
                    ss << "Add wave " << selectedWave->pod.waveSizes.size() + 1;
                    // 새로이 웨이브를 추가하는 부분
                    if (ImGui::Button(ss.str().c_str()))
                        selectedWave->pod.waveSizes.push_back(0);

                    imgui::EndSection();
                }
            }
        }

        void PalettePanel::LoadCallback()
        {
            unitCurrentButton = -1;
            unitButton.clear();

            ornamentCurrentButton = -1;

            auto uSize = tdm.GetDataList(DataType::UnitData).size();
            unitButton.reserve(30);
            for (int i = 0; i < uSize; i++)
            {
                unitButton.push_back(false);
            }
        }
    }
}
