#include "InWanderLand.h"
#include "Module_ScriptEditor.h"

#include "WanderUtils.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorInputManager.h"
#include "EditorPopupManager.h"
#include "Script.h"
#include "Panel_Palette.h"
#include "EditableDataList.h"
#include "EditorCamera.h"

namespace application
{
    namespace editor
    {
        Module_ScriptEditor::Module_ScriptEditor()
        {

        }

        Module_ScriptEditor::~Module_ScriptEditor()
        {

        }

        void Module_ScriptEditor::Initialize()
        {

        }

        void Module_ScriptEditor::Update(float ts)
        {
            static auto& eim = EditorInputManager::GetSingletonInstance();
            static auto& epm = EditorPopupManager::GetSingletonInstance();
            if (!epm.GetReturnPopupName().empty())
            {
                isEditingPopup = true;
            }
            else
            {
                isEditingPopup = false;
            }

            if (isFocused && !isEditingPopup && eim.IsKeyboardPressed(KeyCode::Delete))
            {
                if (selectedAction)
                {
                    selectedScript->EraseAction(selectedAction);
                    selectedAction = nullptr;
                }
                else if (selectedTrigger)
                {
                    selectedScript->EraseTrigger(selectedTrigger);
                    selectedTrigger = nullptr;
                }
                else if (selectedCondition)
                {
                    selectedScript->EraseCondition(selectedCondition);
                    selectedCondition = nullptr;
                }
                else if (selectedScript)
                {
                    editor::EditorLayer::SetInputControl(false);
                    editor::imgui::ShowMessageBox("Delete Script", [&]()
                        {
                            editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                            ImGui::Separator();

                            ImGui::SetNextItemWidth(-1);
                            ImGui::Text(("Are you sure you want to delete the \"" + selectedScript->name + "\"?").c_str());

                            ImGui::Separator();

                            if (ImGui::Button("Delete"))
                            {
                                ScriptSystem::Instance().EraseScript(selectedScript);
                                selectedScript = nullptr;
                                ImGui::CloseCurrentPopup();
                                editor::imgui::CloseMessageBox("Delete Script");
                                editor::EditorLayer::SetInputControl(true);
                            }
                            ImGui::SameLine();

                            if (ImGui::Button("Cancel"))
                            {
                                ImGui::CloseCurrentPopup();
                                editor::imgui::CloseMessageBox("Delete Script");
                                editor::EditorLayer::SetInputControl(true);
                            }
                        }, 500);
                }
            }
        }

        void Module_ScriptEditor::GUIProgress()
        {
            CheckActivation();

            ImGuiWindowFlags flag = 0;
            if (isEditingPopup)
            {
                flag |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus;
                ImGui::SetNextWindowBgAlpha(0.3f);
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.7f));
            }
            ImGui::Begin("Script Editor", &activation, flag);

            /// ImGui 관련 내부 변수 업데이트
            isMouseOver = ImGui::IsWindowHovered();
            isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

            /// 실제 패널에 그리는 영역
            DrawLayout();

            if (isEditingPopup)
            {
                ImGui::PopStyleColor();
            }

            /// Popup 으로 인해 특별한 처리가 필요한 경우
            if (isEditingPopup)
            {
                static auto& epm = EditorPopupManager::GetSingletonInstance();
                static auto& pp = PalettePanel::GetSingletonInstance();

                static auto& pm = palette::PaletteManager::GetSingletonInstance();
                static auto& tp = palette::TerrainPalette::SingleInstance();
                static auto& up = palette::UnitPalette::SingleInstance();
                static auto& op = palette::OrnamentPalette::SingleInstance();
                static auto& rp = palette::RegionPalette::SingleInstance();
                static auto& wp = palette::WavePalette::SingleInstance();
                static auto& cp = palette::CameraPalette::SingleInstance();
                static auto& lp = palette::LightPalette::SingleInstance();
                static auto& ptp = palette::ParticlePalette::SingleInstance();
                static auto& ip = palette::InteractablePalette::SingleInstance();

                bool pop = true;
                ImGuiWindowFlags flag = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
                ImVec2 center = ImGui::GetWindowViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                ImGui::SetNextWindowSize(ImVec2(300, 100));

                /// Trigger
                {
                    /// EnterRegion
                    if (epm.GetReturnPopupName() == "SetEnterRegion")
                    {
                        ImGui::Begin("Enter Region Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Enter Region");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Enter Region");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Region");

                        auto data = epm.GetReturnPopupData<Trigger_EnterRegion>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &rp)
                        {
                            data->isEditing = true;
                            rp.Reset();
                        }

                        if (data->isEditing == true && rp.GetSelections().size() == 1)
                        {
                            data->SetRegion(static_cast<RegionData*>(*rp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    /// LeaveRegion
                    else if (epm.GetReturnPopupName() == "SetLeaveRegion")
                    {
                        ImGui::Begin("Leave Region Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Leave Region");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Leave Region");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Region");

                        auto data = epm.GetReturnPopupData<Trigger_LeaveRegion>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &rp)
                        {
                            data->isEditing = true;
                            rp.Reset();
                        }

                        if (data->isEditing == true && rp.GetSelections().size() == 1)
                        {
                            data->SetRegion(static_cast<RegionData*>(*rp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    /// UnitAppear
                    else if (epm.GetReturnPopupName() == "SetTargetUnit(UnitAppear)")
                    {
                        ImGui::Begin("UnitAppear Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Trigger_UnitAppear>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            data->SetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    /// UnitDie
                    else if (epm.GetReturnPopupName() == "SetTargetUnit(UnitDie)")
                    {
                        ImGui::Begin("UnitDie Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Trigger_UnitDie>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            data->SetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    /// InteractableTriggerEnter
                    else if (epm.GetReturnPopupName() == "SetTargetInteractable(TriggerEnter)")
                    {
                        ImGui::Begin("InteractableTriggerEnter Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Interactable");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Interactable");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Interactable");

                        auto data = epm.GetReturnPopupData<Trigger_InteractableTriggerEnter>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &ip)
                        {
                            data->isEditing = true;
                            ip.Reset();
                        }

                        if (data->isEditing == true && ip.GetSelections().size() == 1)
                        {
                            data->SetInteractable(static_cast<InteractableData*>(*ip.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    /// InteractableTriggerExit
                    else if (epm.GetReturnPopupName() == "SetTargetInteractable(TriggerExit)")
                    {
                        ImGui::Begin("InteractableTriggerExit Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Interactable");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Interactable");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Interactable");

                        auto data = epm.GetReturnPopupData<Trigger_InteractableTriggerExit>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &ip)
                        {
                            data->isEditing = true;
                            ip.Reset();
                        }

                        if (data->isEditing == true && ip.GetSelections().size() == 1)
                        {
                            data->SetInteractable(static_cast<InteractableData*>(*ip.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    /// StartWave
                    else if (epm.GetReturnPopupName() == "SetStartWave")
                    {
                        ImGui::Begin("Start Wave Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Start Wave");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Start Wave");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Wave");

                        auto data = epm.GetReturnPopupData<Trigger_StartWave>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &wp)
                        {
                            data->isEditing = true;
                            wp.Reset();
                        }

                        if (data->isEditing == true && wp.GetSelections().size() == 1)
                        {
                            data->SetWave(static_cast<WaveData*>(*wp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    /// EndWave
                    else if (epm.GetReturnPopupName() == "SetEndWave")
                    {
                        ImGui::Begin("End Wave Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting End Wave");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting End Wave");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Wave");

                        auto data = epm.GetReturnPopupData<Trigger_EndWave>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &wp)
                        {
                            data->isEditing = true;
                            wp.Reset();
                        }

                        if (data->isEditing == true && wp.GetSelections().size() == 1)
                        {
                            data->SetWave(static_cast<WaveData*>(*wp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }
                }

                /// Condition
                {
                    if (epm.GetReturnPopupName() == "SetTargetOrnament(Condition Show)")
                    {
                        ImGui::Begin("Ornament Show Condition(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Ornament");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Ornament");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Ornament");

                        auto data = epm.GetReturnPopupData<Condition_OrnamentShow>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &op)
                        {
                            data->isEditing = true;
                            op.Reset();
                        }

                        if (data->isEditing == true && op.GetSelections().size() == 1)
                        {
                            data->SetTargetOrnament(static_cast<OrnamentData*>(*op.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetOrnament(Condition Hide)")
                    {
                        ImGui::Begin("Ornament Hide Condition(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Ornament");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Ornament");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Ornament");

                        auto data = epm.GetReturnPopupData<Condition_OrnamentHide>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &op)
                        {
                            data->isEditing = true;
                            op.Reset();
                        }

                        if (data->isEditing == true && op.GetSelections().size() == 1)
                        {
                            data->SetTargetOrnament(static_cast<OrnamentData*>(*op.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }
                }

                /// Action
                {
                    if (epm.GetReturnPopupName() == "SetTargetCamera")
                    {
                        ImGui::Begin("Camera Change Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Camera");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Camera");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Cam");

                        auto data = epm.GetReturnPopupData<Action_CameraChangeView>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &cp)
                        {
                            data->isEditing = true;
                            cp.Reset();
                        }

                        if (data->isEditing == true && cp.GetSelections().size() == 1)
                        {
                            data->SetCamera(static_cast<CameraData*>(*cp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetUnit(Move)")
                    {
                        ImGui::Begin("Unit Move Popup(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitMove>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            data->SetTargetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "EditDestinationUnit(Move)")
                    {
                        ImGui::Begin("Unit Move Popup(EditDestination)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        imgui::ShiftCursorY((rect.y - 20) / 2);
                        bool endFlag = false;
                        if (ImGui::Button("End Edit", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
                        {
                            endFlag = true;
                        }
                        ImGui::End();
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitMove>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                            up.SelectUnitInstance(data->destinationUnit);
                        }

                        if (data->isEditing == true && (endFlag || ImGui::IsKeyPressed(ImGuiKey_Escape, false)))
                        {
                            data->isEditing = false;
                            data->destinationUnit->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                            up.Reset();
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            data->destinationUnit->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                            up.Reset();
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetDestinationUnit(Move)")
                    {
                        ImGui::Begin("Unit Move Popup(SetDestination)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Destination Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Destination Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitMove>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            if (data->targetUnit->pod.templateData->pod.skinnedFBXName == static_cast<UnitData*>(*up.GetSelections().begin())->pod.templateData->pod.skinnedFBXName)
                            {
                                data->SetDestinationUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                                data->isEditing = false;
                                up.Reset();
                                epm.Return();
                            }
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetUnit(Rotate)")
                    {
                        ImGui::Begin("Unit Rotate Popup(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitRotate>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            data->SetTargetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetUnit(Rescale)")
                    {
                        ImGui::Begin("Unit Rescale Popup(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitRescale>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            data->SetTargetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetUnit(MoveWithRotateAndRescale)")
                    {
                        ImGui::Begin("Unit Move With Rotate And Rescale Popup(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitMoveWithRotateAndRescale>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            data->SetTargetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "EditDestinationUnit(MoveWithRotateAndRescale)")
                    {
                        ImGui::Begin("Unit Move With Rotate And Rescale Popup(EditDestination)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        imgui::ShiftCursorY((rect.y - 20) / 2);
                        bool endFlag = false;
                        if (ImGui::Button("End Edit", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
                        {
                            endFlag = true;
                        }
                        ImGui::End();
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitMoveWithRotateAndRescale>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                            up.SelectUnitInstance(data->destinationUnit);
                        }

                        if (data->isEditing == true && (endFlag || ImGui::IsKeyPressed(ImGuiKey_Escape, false)))
                        {
                            data->isEditing = false;
                            data->destinationUnit->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                            up.Reset();
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            data->destinationUnit->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                            up.Reset();
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetDestinationUnit(MoveWithRotateAndRescale)")
                    {
                        ImGui::Begin("Unit Move With Rotate And Rescale Popup(SetDestination)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Destination Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Destination Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitMoveWithRotateAndRescale>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            if (data->targetUnit->pod.templateData->pod.skinnedFBXName == static_cast<UnitData*>(*up.GetSelections().begin())->pod.templateData->pod.skinnedFBXName)
                            {
                                data->SetDestinationUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                                data->isEditing = false;
                                up.Reset();
                                epm.Return();
                            }
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetUnit(Relocate)")
                    {
                        ImGui::Begin("Unit Relocate Popup(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitRelocate>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            data->SetTargetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "EditDestinationUnit(Relocate)")
                    {
                        ImGui::Begin("Unit Relocate Popup(EditDestination)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        imgui::ShiftCursorY((rect.y - 20) / 2);
                        bool endFlag = false;
                        if (ImGui::Button("End Edit", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
                        {
                            endFlag = true;
                        }
                        ImGui::End();
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitRelocate>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                            up.SelectUnitInstance(data->destinationUnit);
                        }

                        if (data->isEditing == true && (endFlag || ImGui::IsKeyPressed(ImGuiKey_Escape, false)))
                        {
                            data->isEditing = false;
                            data->destinationUnit->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                            up.Reset();
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            data->destinationUnit->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                            up.Reset();
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetDestinationUnit(Relocate)")
                    {
                        ImGui::Begin("Unit Relocate Popup(SetDestination)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Destination Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Destination Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitRelocate>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            if (data->targetUnit->pod.templateData->pod.skinnedFBXName == static_cast<UnitData*>(*up.GetSelections().begin())->pod.templateData->pod.skinnedFBXName)
                            {
                                data->SetDestinationUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                                data->isEditing = false;
                                up.Reset();
                                epm.Return();
                            }
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetOrnament(MoveWithRotateAndRescale)")
                    {
                        ImGui::Begin("Ornament Move With Rotate And Rescale Popup(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Ornament");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Ornament");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Ornament");

                        auto data = epm.GetReturnPopupData<Action_OrnamentMoveWithRotateAndRescale>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &op)
                        {
                            data->isEditing = true;
                            op.Reset();
                        }

                        if (data->isEditing == true && op.GetSelections().size() == 1)
                        {
                            data->SetTargetOrnament(static_cast<OrnamentData*>(*op.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "EditDestinationOrnament(MoveWithRotateAndRescale)")
                    {
                        ImGui::Begin("Ornament Move With Rotate And Rescale Popup(EditDestination)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        imgui::ShiftCursorY((rect.y - 20) / 2);
                        bool endFlag = false;
                        if (ImGui::Button("End Edit", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
                        {
                            endFlag = true;
                        }
                        ImGui::End();
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());

                        pp.ChangeTab("Ornament");

                        auto data = epm.GetReturnPopupData<Action_OrnamentMoveWithRotateAndRescale>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &op)
                        {
                            data->isEditing = true;
                            op.Reset();
                            op.SelectOrnamentInstance(data->destinationOrnament);
                        }

                        if (data->isEditing == true && (endFlag || ImGui::IsKeyPressed(ImGuiKey_Escape, false)))
                        {
                            data->isEditing = false;
                            data->destinationOrnament->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                            op.Reset();
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            data->destinationOrnament->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                            op.Reset();
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetDestinationOrnament(MoveWithRotateAndRescale)")
                    {
                        ImGui::Begin("Ornament Move With Rotate And Rescale Popup(SetDestination)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Destination Ornament");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Destination Ornament");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Ornament");

                        auto data = epm.GetReturnPopupData<Action_OrnamentMoveWithRotateAndRescale>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &op)
                        {
                            data->isEditing = true;
                            op.Reset();
                        }

                        if (data->isEditing == true && op.GetSelections().size() == 1)
                        {
                            if (data->targetOrnament->pod.templateData->pod.staticFBXName == static_cast<OrnamentData*>(*op.GetSelections().begin())->pod.templateData->pod.staticFBXName)
                            {
                                data->SetDestinationOrnament(static_cast<OrnamentData*>(*op.GetSelections().begin()));
                                data->isEditing = false;
                                op.Reset();
                                epm.Return();
                            }
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetOrnament(Show)")
                    {
                        ImGui::Begin("Ornament Show Popup(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Ornament");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Ornament");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Ornament");

                        auto data = epm.GetReturnPopupData<Action_OrnamentShow>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &op)
                        {
                            data->isEditing = true;
                            op.Reset();
                        }

                        if (data->isEditing == true && op.GetSelections().size() == 1)
                        {
                            data->SetTargetOrnament(static_cast<OrnamentData*>(*op.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetOrnament(Hide)")
                    {
                        ImGui::Begin("Ornament Hide Popup(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Ornament");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Ornament");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Ornament");

                        auto data = epm.GetReturnPopupData<Action_OrnamentHide>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &op)
                        {
                            data->isEditing = true;
                            op.Reset();
                        }

                        if (data->isEditing == true && op.GetSelections().size() == 1)
                        {
                            data->SetTargetOrnament(static_cast<OrnamentData*>(*op.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetOrnament(Floating)")
                    {
                        ImGui::Begin("Ornament Floating Popup(SetTarget)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Ornament");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Ornament");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Ornament");

                        auto data = epm.GetReturnPopupData<Action_OrnamentFloating>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &op)
                        {
                            data->isEditing = true;
                            op.Reset();
                        }

                        if (data->isEditing == true && op.GetSelections().size() == 1)
                        {
                            data->SetTargetOrnament(static_cast<OrnamentData*>(*op.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetUnit(ForcePause)")
                    {
                        ImGui::Begin("Unit Force Pause Popup(Set Unit)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_ForceUnitPaused>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            data->SetTargetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetUnit(ForceUnpause)")
                    {
                        ImGui::Begin("Unit Force Unpause Popup(Set Unit)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_ForceUnitUnpaused>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            data->SetTargetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetParticle(MoveWithRotateAndRescale)")
                    {
                        ImGui::Begin("Particle MoveWithRotateAndRescale Popup(Set Target)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Particle");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Particle");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Particle");

                        auto data = epm.GetReturnPopupData<Action_ParticleMoveWithRotateAndRescale>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &ptp)
                        {
                            data->isEditing = true;
                            ptp.Reset();
                        }

                        if (data->isEditing == true && ptp.GetSelections().size() == 1)
                        {
                            data->SetTargetParticle(static_cast<ParticleData*>(*ptp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "EditDestinationParticle(MoveWithRotateAndRescale)")
                    {
                        ImGui::Begin("Particle MoveWithRotateAndRescale Popup(Edit Destination)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        imgui::ShiftCursorY((rect.y - 20) / 2);
                        bool endFlag = false;
                        if (ImGui::Button("End Edit", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
                        {
                            endFlag = true;
                        }
                        ImGui::End();
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());

                        pp.ChangeTab("Particle");

                        auto data = epm.GetReturnPopupData<Action_ParticleMoveWithRotateAndRescale>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &ptp)
                        {
                            data->isEditing = true;
                            ptp.Reset();
                            ptp.SelectParticle(data->destinationParticle);
                        }

                        if (data->isEditing == true && (endFlag || ImGui::IsKeyPressed(ImGuiKey_Escape, false)))
                        {
                            data->isEditing = false;
                            data->destinationParticle->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                            ptp.Reset();
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            data->destinationParticle->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                            ptp.Reset();
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetDestinationParticle(MoveWithRotateAndRescale)")
                    {
                        ImGui::Begin("Particle MoveWithRotateAndRescale Popup(Set Destination)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Destination Particle");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Destination Particle");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Particle");

                        auto data = epm.GetReturnPopupData<Action_ParticleMoveWithRotateAndRescale>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &ptp)
                        {
                            data->isEditing = true;
                            ptp.Reset();
                        }

                        if (data->isEditing == true && ptp.GetSelections().size() == 1)
                        {
                            data->SetDestinationParticle(static_cast<ParticleData*>(*ptp.GetSelections().begin()));
                            data->isEditing = false;
                            ptp.Reset();
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetParticle(Show)")
                    {
                        ImGui::Begin("Particle Show Popup(Set Target)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Particle");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Particle");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Particle");

                        auto data = epm.GetReturnPopupData<Action_ParticleShow>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &ptp)
                        {
                            data->isEditing = true;
                            ptp.Reset();
                        }

                        if (data->isEditing == true && ptp.GetSelections().size() == 1)
                        {
                            data->SetTargetParticle(static_cast<ParticleData*>(*ptp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetParticle(Hide)")
                    {
                        ImGui::Begin("Particle Hide Popup(Set Target)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Particle");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Particle");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Particle");

                        auto data = epm.GetReturnPopupData<Action_ParticleHide>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &ptp)
                        {
                            data->isEditing = true;
                            ptp.Reset();
                        }

                        if (data->isEditing == true && ptp.GetSelections().size() == 1)
                        {
                            data->SetTargetParticle(static_cast<ParticleData*>(*ptp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetUnit(PlayAnimation)")
                    {
                        ImGui::Begin("Unit Play Animation Popup(Set Unit)", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Target Unit");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Target Unit");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Unit");

                        auto data = epm.GetReturnPopupData<Action_UnitPlayAnimation>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &up)
                        {
                            data->isEditing = true;
                            up.Reset();
                        }

                        if (data->isEditing == true && up.GetSelections().size() == 1)
                        {
                            data->SetTargetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetCamera(Tactic)")
                    {
                        ImGui::Begin("Tactic Camera Setting Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Camera");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Camera");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Cam");

                        auto data = epm.GetReturnPopupData<Action_SetTacticCamera>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &cp)
                        {
                            data->isEditing = true;
                            cp.Reset();
                        }

                        if (data->isEditing == true && cp.GetSelections().size() == 1)
                        {
                            data->SetCamera(static_cast<CameraData*>(*cp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetRegion(SetAsNavObstacle)")
                    {
                        ImGui::Begin("Nav Obstacle Region Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Region");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Region");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Region");

                        auto data = epm.GetReturnPopupData<Action_SetRegionAsNavObstacle>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &rp)
                        {
                            data->isEditing = true;
                            rp.Reset();
                        }

                        if (data->isEditing == true && rp.GetSelections().size() == 1)
                        {
                            data->SetTargetRegion(static_cast<RegionData*>(*rp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }

                    if (epm.GetReturnPopupName() == "SetTargetRegion(PlayerConstraining)")
                    {
                        ImGui::Begin("Player Constraining Region Popup", &pop, flag);
                        auto rect = ImGui::GetContentRegionAvail();
                        auto size = ImGui::CalcTextSize("Please Setting Region");
                        imgui::ShiftCursorX((rect.x - size.x) / 2);
                        imgui::ShiftCursorY((rect.y - size.y) / 2);
                        ImGui::Text("Please Setting Region");
                        ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
                        ImGui::End();

                        pp.ChangeTab("Region");

                        auto data = epm.GetReturnPopupData<Action_SetPlayerConstrainingRegion>();
                        if (data->isEditing == false && pm.GetCurrentPalette() == &rp)
                        {
                            data->isEditing = true;
                            rp.Reset();
                        }

                        if (data->isEditing == true && rp.GetSelections().size() == 1)
                        {
                            data->SetTargetRegion(static_cast<RegionData*>(*rp.GetSelections().begin()));
                            data->isEditing = false;
                            epm.Return();
                        }

                        if (!pop)
                        {
                            data->isEditing = false;
                            epm.Return();
                        }
                    }
                }
            }
            ImGui::End();
        }

        void Module_ScriptEditor::Finalize()
        {

        }

        void Module_ScriptEditor::Clear()
        {
            selectedScript = nullptr;
            selectedTrigger = nullptr;
            selectedCondition = nullptr;
            selectedAction = nullptr;
            isEditingPopup = false;
        }

        Script* Module_ScriptEditor::GetSelectedScript()
        {
            return selectedScript;
        }

        void Module_ScriptEditor::DrawLayout()
        {
            // Spacing, 바꾸지 않는다는 전제로 static으로 구현
            static auto spacing = ImGui::GetStyle().ItemSpacing;

            auto region = ImGui::GetContentRegionAvail();

            DrawList(ImVec2(region.x * 0.2 - spacing.x / 2, region.y - spacing.y / 2));
            ImGui::SameLine();
            DrawScriptPanel(ImVec2(region.x * 0.8 - spacing.x / 2, region.y - spacing.y / 2));
        }

        void Module_ScriptEditor::DrawList(const ImVec2& region)
        {
            ImGuiWindowFlags flag = 0;
            if (isEditingPopup)
            {
                flag |= ImGuiWindowFlags_NoInputs;
            }
            ImGui::BeginChild("ScriptList", region, true, flag);
            if (ImGui::Button("Create Script", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
            {
                selectedScript = ScriptSystem::Instance().CreateScript();
            }

            imgui::ShiftCursorY(10);

            auto selectedData = GetSelectedScript();

            int idx = 0;
            std::vector<Script*> sortedScriptList{ ScriptSystem::Instance().GetScriptList().begin(), ScriptSystem::Instance().GetScriptList().end() };
            std::sort(sortedScriptList.begin(), sortedScriptList.end(), [](Script* a, Script* b) { return a->name < b->name; });
            for (auto each : sortedScriptList)
            {
                ImGui::PushID(("Script_Name" + each->name + to_string(idx)).c_str());

                if (ImGui::Selectable(each->name.c_str(), selectedData == each))
                {
                    if (selectedScript == each)
                    {
                        selectedScript = nullptr;
                    }
                    else
                    {
                        selectedScript = each;
                    }
                    selectedTrigger = nullptr;
                    selectedCondition = nullptr;
                    selectedAction = nullptr;
                }
                ImGui::PopID();
                idx++;
            }

            ImGui::EndChild();
        }

        void Module_ScriptEditor::DrawScriptPanel(const ImVec2& region)
        {
            ImGuiWindowFlags flag = 0;
            if (isEditingPopup)
            {
                flag |= ImGuiWindowFlags_NoInputs;
            }
            ImGui::BeginChild("SelectedScript", region, true, flag);

            auto selectedData = GetSelectedScript();

            if (selectedData != nullptr)
            {
                DrawScript(selectedData);
            }
            ImGui::EndChild();
        }

        void Module_ScriptEditor::DrawScript(Script* data)
        {
            std::string scriptName = data->name;
            scriptName.reserve(32);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5);
            ImGui::InputText("##Script_Name", &scriptName[0], 32);

            static bool beforeFocus = false;
            if (ImGui::IsItemFocused())
            {
                beforeFocus = true;
                EditorLayer::SetInputControl(false);
                EditorCamera::GetSingletonInstance().SetInputUpdate(false);
            }
            else if (beforeFocus)
            {
                beforeFocus = false;
                EditorLayer::SetInputControl(true);
                EditorCamera::GetSingletonInstance().SetInputUpdate(true);
            }

            wanderUtils::UpdateStringSize(scriptName);
            data->name = scriptName;

            ImGui::SameLine();
            if (ImGui::Button("Add Trigger"))
            {
                ImGui_AddTriggerPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Add Condition"))
            {
                ImGui_AddConditionPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Add Action"))
            {
                ImGui_AddActionPopup();
            }
            imgui::ShiftCursorY(10);

            if (ImGui::CollapsingHeader("Triggers"))
            {
                for (auto& each : data->triggerList)
                {
                    DrawTrigger(each);
                }
            }
            else
            {
                selectedTrigger = nullptr;
            }
            imgui::ShiftCursorY(10);

            if (ImGui::CollapsingHeader("Conditions"))
            {
                for (auto& each : data->conditionList)
                {
                    DrawCondition(each);
                }
            }
            else
            {
                selectedCondition = nullptr;
            }
            imgui::ShiftCursorY(10);

            if (ImGui::CollapsingHeader("Actions"))
            {
                int finalIdx = -1;
                int offset = 0;
                int idx = 0;
                for (auto& action : data->actionList)
                {
                    DrawAction(action);
                    offset = ImGui::ItemReorder("ActionReorder", true);
                    if (offset != 0 && idx + offset >= 0)
                    {
                        finalIdx = idx + offset;
                    }
                    idx++;
                }

                if (finalIdx >= 0)
                {
                    selectedScript->ReorderAction(selectedAction, finalIdx);
                }
            }
            else
            {
                selectedAction = nullptr;
            }
        }

        void Module_ScriptEditor::ImGui_AddTriggerPopup()
        {
            EditorLayer::SetInputControl(false);
            imgui::ShowMessageBox("Add Trigger", [this]()
                {
                    imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    static std::string triggerName = "None";
                    static TriggerType triggerType = TriggerType::None;

                    ImGui::SetNextItemWidth(-1);
                    if (ImGui::BeginCombo("##TriggerListCombo", triggerName.c_str(), ImGuiComboFlags_HeightLargest))
                    {
                        for (auto& [type, str] : ScriptSystem::triggerList)
                        {
                            const bool is_selected = (triggerName == str);
                            if (ImGui::Selectable(str.c_str(), is_selected))
                            {
                                triggerName = str;
                                triggerType = type;
                            }

                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Add"))
                    {
                        if (triggerName != "None")
                        {
                            switch (triggerType)
                            {
                            case application::TriggerType::GameStart:
                            {
                                selectedScript->AddTrigger<Trigger_GameStart>();
                                break;
                            }
                            case application::TriggerType::EnterRegion:
                            {
                                selectedScript->AddTrigger<Trigger_EnterRegion>();
                                break;
                            }
                            case application::TriggerType::LeaveRegion:
                            {
                                selectedScript->AddTrigger<Trigger_LeaveRegion>();
                                break;
                            }
                            case application::TriggerType::RepeatPeriodically:
                            {
                                selectedScript->AddTrigger<Trigger_RepeatPeriodically>();
                                break;
                            }
                            case application::TriggerType::RepeatPeriodicallyRealTime:
                            {
                                selectedScript->AddTrigger<Trigger_RepeatPeriodicallyRealTime>();
                                break;
                            }
                            case application::TriggerType::UnitAppear:
                            {
                                selectedScript->AddTrigger<Trigger_UnitAppear>();
                                break;
                            }
                            case application::TriggerType::UnitDie:
                            {
                                selectedScript->AddTrigger<Trigger_UnitDie>();
                                break;
                            }
                            case application::TriggerType::InteractableTriggerEnter:
                            {
                                selectedScript->AddTrigger<Trigger_InteractableTriggerEnter>();
                                break;
                            }
                            case application::TriggerType::InteractableTriggerExit:
                            {
                                selectedScript->AddTrigger<Trigger_InteractableTriggerExit>();
                                break;
                            }
                            case application::TriggerType::StartWave:
                            {
                                selectedScript->AddTrigger<Trigger_StartWave>();
                                break;
                            }
                            case application::TriggerType::EndWave:
                            {
                                selectedScript->AddTrigger<Trigger_EndWave>();
                                break;
                            }
                            case application::TriggerType::StageStart:
                            {
                                selectedScript->AddTrigger<Trigger_StageStart>();
                                break;
                            }
                            default:
                                break;
                            }

                            triggerName = "None";
                            triggerType = TriggerType::None;
                            ImGui::CloseCurrentPopup();
                            imgui::CloseMessageBox("Add Trigger");
                            EditorLayer::SetInputControl(true);
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        triggerName = "None";
                        triggerType = TriggerType::None;
                        ImGui::CloseCurrentPopup();
                        imgui::CloseMessageBox("Add Trigger");
                        EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }

        void Module_ScriptEditor::ImGui_AddConditionPopup()
        {
            EditorLayer::SetInputControl(false);
            imgui::ShowMessageBox("Add Condition", [this]()
                {
                    imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    static std::string conditionName = "None";
                    static ConditionType conditionType = ConditionType::None;

                    ImGui::SetNextItemWidth(-1);
                    if (ImGui::BeginCombo("##ConditionListCombo", conditionName.c_str(), ImGuiComboFlags_HeightLargest))
                    {
                        for (auto& [type, str] : ScriptSystem::conditionList)
                        {
                            const bool is_selected = (conditionName == str);
                            if (ImGui::Selectable(str.c_str(), is_selected))
                            {
                                conditionName = str;
                                conditionType = type;
                            }

                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Add"))
                    {
                        if (conditionName != "None")
                        {
                            switch (conditionType)
                            {
                            case application::ConditionType::CinematicModeOn:
                            {
                                selectedScript->AddCondition<Condition_CinematicModeOn>();
                                break;
                            }
                            case application::ConditionType::CinematicModeOff:
                            {
                                selectedScript->AddCondition<Condition_CinematicModeOff>();
                                break;
                            }
                            case application::ConditionType::TutorialModeOn:
                            {
                                selectedScript->AddCondition<Condition_TutorialModeOn>();
                                break;
                            }
                            case application::ConditionType::TutorialModeOff:
                            {
                                selectedScript->AddCondition<Condition_TutorialModeOff>();
                                break;
                            }
                            case application::ConditionType::TacticModeOn:
                            {
                                selectedScript->AddCondition<Condition_TacticModeOn>();
                                break;
                            }
                            case application::ConditionType::TacticModeOff:
                            {
                                selectedScript->AddCondition<Condition_TacticModeOff>();
                                break;
                            }
                            case application::ConditionType::PlayerState:
                            {
                                selectedScript->AddCondition<Condition_PlayerState>();
                                break;
                            }
                            case application::ConditionType::OrnamentShow:
                            {
                                selectedScript->AddCondition<Condition_OrnamentShow>();
                                break;
                            }
                            case application::ConditionType::OrnamentHide:
                            {
                                selectedScript->AddCondition<Condition_OrnamentHide>();
                                break;
                            }
                            case application::ConditionType::Stage:
                            {
                                selectedScript->AddCondition<Condition_Stage>();
                                break;
                            }
                            default:
                                break;
                            }

                            conditionName = "None";
                            conditionType = ConditionType::None;
                            ImGui::CloseCurrentPopup();
                            imgui::CloseMessageBox("Add Condition");
                            EditorLayer::SetInputControl(true);
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        conditionName = "None";
                        conditionType = ConditionType::None;
                        ImGui::CloseCurrentPopup();
                        imgui::CloseMessageBox("Add Condition");
                        EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }

        void Module_ScriptEditor::ImGui_AddActionPopup()
        {
            EditorLayer::SetInputControl(false);
            imgui::ShowMessageBox("Add Action", [this]()
                {
                    imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    static std::string actionName = "None";
                    static ActionType actionType = ActionType::None;

                    ImGui::SetNextItemWidth(-1);
                    if (ImGui::BeginCombo("##ActionListCombo", actionName.c_str()), ImGuiComboFlags_HeightLargest)
                    {
                        for (auto& [type, str] : ScriptSystem::actionList)
                        {
                            const bool is_selected = (actionName == str);
                            if (ImGui::Selectable(str.c_str(), is_selected))
                            {
                                actionName = str;
                                actionType = type;
                            }

                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Add"))
                    {
                        if (actionName != "None")
                        {
                            switch (actionType)
                            {
                            case application::ActionType::WaitForSeconds:
                            {
                                selectedScript->AddAction<Action_WaitForSeconds>();
                                break;
                            }
                            case application::ActionType::WaitForRealSeconds:
                            {
                                selectedScript->AddAction<Action_WaitForRealSeconds>();
                                break;
                            }
                            case application::ActionType::CinematicModeChange:
                            {
                                selectedScript->AddAction<Action_CinematicModeChange>();
                                break;
                            }
                            case application::ActionType::CameraChangeView:
                            {
                                selectedScript->AddAction<Action_CameraChangeView>();
                                break;
                            }
                            case application::ActionType::CameraSaveView:
                            {
                                selectedScript->AddAction<Action_CameraSaveView>();
                                break;
                            }
                            case application::ActionType::CameraLoadView:
                            {
                                selectedScript->AddAction<Action_CameraLoadView>();
                                break;
                            }
                            case application::ActionType::CinematicFadeIn:
                            {
                                selectedScript->AddAction<Action_CinematicFadeIn>();
                                break;
                            }
                            case application::ActionType::CinematicFadeOut:
                            {
                                selectedScript->AddAction<Action_CinematicFadeOut>();
                                break;
                            }
                            case application::ActionType::WaitPreviousActionEnd:
                            {
                                selectedScript->AddAction<Action_WaitPreviousActionEnd>();
                                break;
                            }
                            case application::ActionType::CameraRevert:
                            {
                                selectedScript->AddAction<Action_CameraRevert>();
                                break;
                            }
                            case application::ActionType::TutorialModeChange:
                            {
                                selectedScript->AddAction<Action_TutorialModeChange>();
                                break;
                            }
                            case application::ActionType::PlayManualDialogue:
                            {
                                selectedScript->AddAction<Action_PlayManualDialogue>();
                                break;
                            }
                            case application::ActionType::PlayTimedDialogue:
                            {
                                selectedScript->AddAction<Action_PlayTimedDialogue>();
                                break;
                            }
                            case application::ActionType::UnitMove:
                            {
                                selectedScript->AddAction<Action_UnitMove>();
                                break;
                            }
                            case application::ActionType::UnitRotate:
                            {
                                selectedScript->AddAction<Action_UnitRotate>();
                                break;
                            }
                            case application::ActionType::UnitRescale:
                            {
                                selectedScript->AddAction<Action_UnitRescale>();
                                break;
                            }
                            case application::ActionType::UnitMoveWithRotateAndRescale:
                            {
                                selectedScript->AddAction<Action_UnitMoveWithRotateAndRescale>();
                                break;
                            }
                            case application::ActionType::UnitRelocate:
                            {
                                selectedScript->AddAction<Action_UnitRelocate>();
                                break;
                            }
                            case application::ActionType::UnitPauseAll:
                            {
                                selectedScript->AddAction<Action_UnitPauseAll>();
                                break;
                            }
                            case application::ActionType::PlayerSelect:
                            {
                                selectedScript->AddAction<Action_PlayerSelect>();
                                break;
                            }
                            case application::ActionType::BlockPlayerSwitch:
                            {
                                selectedScript->AddAction<Action_BlockPlayerSwitch>();
                                break;
                            }
                            case application::ActionType::BlockSkillCancel:
                            {
                                selectedScript->AddAction<Action_BlockSkillCancel>();
                                break;
                            }
                            case application::ActionType::BlockSkillSelection:
                            {
                                selectedScript->AddAction<Action_BlockSkillSelection>();
                                break;
                            }
                            case application::ActionType::AwaitSkillSelection:
                            {
                                selectedScript->AddAction<Action_AwaitSkillSelection>();
                                break;
                            }
                            case application::ActionType::AwaitSkillActivation:
                            {
                                selectedScript->AddAction<Action_AwaitSkillActivation>();
                                break;
                            }
                            case application::ActionType::SetTimeScale:
                            {
                                selectedScript->AddAction<Action_SetTimeScale>();
                                break;
                            }
                            case application::ActionType::PauseWaves:
                            {
                                selectedScript->AddAction<Action_PauseWaves>();
                                break;
                            }
                            case application::ActionType::EngageStage:
                            {
                                selectedScript->AddAction<Action_EngageStage>();
                                break;
                            }
                            case application::ActionType::SoundPlay:
                            {
                                selectedScript->AddAction<Action_SoundPlay>();
                                break;
                            }
                            case application::ActionType::SoundPlayMusic:
                            {
                                selectedScript->AddAction<Action_SoundPlayMusic>();
                                break;
                            }
                            case application::ActionType::SoundSetMusicVolume:
                            {
                                selectedScript->AddAction<Action_SoundSetMusicVolume>();
                                break;
                            }
                            case application::ActionType::UISetActive:
                            {
                                selectedScript->AddAction<Action_UISetActive>();
                                break;
                            }
                            case application::ActionType::OrnamentMoveWithRotateAndRescale:
                            {
                                selectedScript->AddAction<Action_OrnamentMoveWithRotateAndRescale>();
                                break;
                            }
                            case application::ActionType::OrnamentShow:
                            {
                                selectedScript->AddAction<Action_OrnamentShow>();
                                break;
                            }
                            case application::ActionType::OrnamentHide:
                            {
                                selectedScript->AddAction<Action_OrnamentHide>();
                                break;
                            }
                            case application::ActionType::OrnamentFloating:
                            {
                                selectedScript->AddAction<Action_OrnamentFloating>();
                                break;
                            }
                            case application::ActionType::BossAppear:
                            {
                                selectedScript->AddAction<Action_BossAppear>();
                                break;
                            }
                            case application::ActionType::RequestState:
                            {
                                selectedScript->AddAction<Action_RequestState>();
                                break;
                            }
                            case application::ActionType::AwaitSkillTargeting:
                            {
                                selectedScript->AddAction<Action_AwaitSkillTargeting>();
                                break;
                            }

                            case application::ActionType::AwaitSkillExpiration:
                            {
                                selectedScript->AddAction<Action_AwaitSkillExpiration>();
                                break;
                            }

                            case application::ActionType::ForceUnitPaused:
                            {
                                selectedScript->AddAction<Action_ForceUnitPaused>();
                                break;
                            }

                            case application::ActionType::ForceUnitUnpaused:
                            {
                                selectedScript->AddAction<Action_ForceUnitUnpaused>();
                                break;
                            }

                            case application::ActionType::SetZoomFactor:
                            {
                                selectedScript->AddAction<Action_SetZoomFactor>();
                                break;
                            }
                            case application::ActionType::SetCamBidirectionalPath:
                            {
                                selectedScript->AddAction<Action_SetCamBidirectionalPath>();
                                break;
                            }
                            case application::ActionType::ParticleMoveWithRotateAndRescale:
                            {
                                selectedScript->AddAction<Action_ParticleMoveWithRotateAndRescale>();
                                break;
                            }
                            case application::ActionType::ParticleShow:
                            {
                                selectedScript->AddAction<Action_ParticleShow>();
                                break;
                            }
                            case application::ActionType::ParticleHide:
                            {
                                selectedScript->AddAction<Action_ParticleHide>();
                                break;
                            }
                            case application::ActionType::UnitPlayAnimation:
                            {
                                selectedScript->AddAction<Action_UnitPlayAnimation>();
                                break;
                            }
                            case application::ActionType::PullScriptTrigger:
                            {
                                selectedScript->AddAction<Action_PullScriptTrigger>();
                                break;
                            }
                            case application::ActionType::SetTacticCamera:
                            {
                                selectedScript->AddAction<Action_SetTacticCamera>();
                                break;
                            }
                            case application::ActionType::SetRegionAsNavObstacle:
                            {
                                selectedScript->AddAction<Action_SetRegionAsNavObstacle>();
                                break;
                            }
                            case application::ActionType::SetPlayerConstrainingRegion:
                            {
                                selectedScript->AddAction<Action_SetPlayerConstrainingRegion>();
                                break;
                            }
                            default:
                                break;
                            }

                            actionName = "None";
                            actionType = ActionType::None;
                            ImGui::CloseCurrentPopup();
                            imgui::CloseMessageBox("Add Action");
                            EditorLayer::SetInputControl(true);
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        actionName = "None";
                        actionType = ActionType::None;
                        ImGui::CloseCurrentPopup();
                        imgui::CloseMessageBox("Add Action");
                        EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }

        void Module_ScriptEditor::DrawTrigger(std::shared_ptr<ITrigger> data)
        {
            if (!data->IsValid())
            {
                auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
                ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + ImGui::CalcTextSize("Font").y), IM_COL32(255, 20, 20, 255), false, 0.0f);
                ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(255, 20, 20, 255));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(200, 20, 20, 255));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255, 20, 20, 255));
            }

            ImGui::PushID(data.get());
            if (ImGui::Selectable(data->GetTypeName().c_str(), data == selectedTrigger))
            {
                if (selectedTrigger == data)
                {
                    selectedTrigger = nullptr;
                }
                else
                {
                    selectedTrigger = data;
                }
                selectedCondition = nullptr;
                selectedAction = nullptr;
            }

            if (!data->IsValid())
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }

            if ((selectedTrigger == data) && ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup(data->GetTypeName().c_str());
            }

            PopUpDataEdit<ITrigger>(data.get());

            ImGui::PopID();
        }

        void Module_ScriptEditor::DrawCondition(std::shared_ptr<ICondition> data)
        {
            if (!data->IsValid())
            {
                auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
                ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + ImGui::CalcTextSize("Font").y), IM_COL32(255, 20, 20, 255), false, 0.0f);
                ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(255, 20, 20, 255));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(200, 20, 20, 255));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255, 20, 20, 255));
            }

            ImGui::PushID(data.get());
            if (ImGui::Selectable(data->GetTypeName().c_str(), data == selectedCondition))
            {
                if (selectedCondition == data)
                {
                    selectedCondition = nullptr;
                }
                else
                {
                    selectedCondition = data;
                }
                selectedTrigger = nullptr;
                selectedAction = nullptr;
            }

            if (!data->IsValid())
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }

            if ((selectedCondition == data) && ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup(data->GetTypeName().c_str());
            }

            PopUpDataEdit<ICondition>(data.get());

            ImGui::PopID();
        }

        void Module_ScriptEditor::DrawAction(std::shared_ptr<IAction> data)
        {
            if (!data->IsValid())
            {
                auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
                ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + ImGui::CalcTextSize("Font").y), IM_COL32(255, 20, 20, 255), false, 0.0f);
                ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(255, 20, 20, 255));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(200, 20, 20, 255));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255, 20, 20, 255));
            }

            ImGui::PushID(data.get());
            if (ImGui::Selectable(data->GetTypeName().c_str(), data == selectedAction))
            {
                if (selectedAction == data)
                {
                    selectedAction = nullptr;
                }
                else
                {
                    selectedAction = data;
                }
                selectedTrigger = nullptr;
                selectedCondition = nullptr;
            }

            if (!data->IsValid())
            {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }

            if ((selectedAction == data) && ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup(data->GetTypeName().c_str());
            }

            PopUpDataEdit<IAction>(data.get());

            ImGui::PopID();
        }
    }
}
