#include "InWanderLand.h"
#include "Panel_SceneView.h"
#include "Application.h"
#include "WindowEvents.h"
#include "MouseEvents.h"
#include "PaletteList.h"
#include "imgui_Utility.h"
#include "EditorCamera.h"
#include "ImGuizmo/ImGuizmo.h"
#include "EditorMath.h"
#include "EditorLayer.h"
#include "CommandManager.h"

#include "imgui.h"
#include "imgui_internal.h"

#include <string>
#include <d3d11.h>

#include <iostream>

namespace application
{
    namespace editor
    {
        SceneViewPanel::SceneViewPanel()
            : app(nullptr), ec(nullptr), pm(nullptr), prevWindowSize(), currentWindowSize(), imageStartPos(), renderImageSize(), cursorPos_InScreenSpace()
        {
            beforeTS.reserve(30);
        }

        SceneViewPanel::~SceneViewPanel()
        {

        }

        void SceneViewPanel::Initialize()
        {
            app = &Application::GetInstance();
            ec = &EditorCamera::GetSingletonInstance();
            pm = &palette::PaletteManager::GetSingletonInstance();
        }

        void SceneViewPanel::Update(float ts)
        {

        }

        void SceneViewPanel::GUIProgress()
        {
            ImGui::Begin("SceneView", 0, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar);

            ImGui_Update();

            /// 실제 패널에 그리는 영역
            ImGui_SceneViewSettings();

            ImGui_DrawMenuBar();

            ImGui::Image(reinterpret_cast<ImTextureID>(app->GetSceneSRV()), ImVec2(renderImageSize.first, renderImageSize.second));

            ImGui_DrawGizmo();

            ImGui::End();
        }

        void SceneViewPanel::Finalize()
        {

        }

        void SceneViewPanel::OnEvent(EditorEvents& event)
        {
            EventDispatcher dispatcher(event);


        }

        std::pair<unsigned int, unsigned int> SceneViewPanel::GetRenderImageSize() const
        {
            return renderImageSize;
        }

        std::pair<double, double> SceneViewPanel::GetCursorPosInScreenSpace() const
        {
            return cursorPos_InScreenSpace;
        }

        bool SceneViewPanel::IsMouseOverGizmo()
        {
            return pm->GetCurrentPalette()->AreThereAnyObjectSelected() && ImGuizmo::IsOver();
        }

        void SceneViewPanel::ImGui_Update()
        {
            /// ImGui 관련 내부 변수 업데이트
            isMouseOver = ImGui::IsWindowHovered();
            isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

            ImGui_UpdateWindowSize();

            // Resize 에 대한 처리 부분
            if (ImGui_IsWindowResize())
            {
                ImGui_OnResizeRenderImageSize();
            }

            if (Application::IsFocusGameWindow() == false && EditorLayer::GetInputControl())
            {
                // 마우스 입력에 대한 처리
                if (isMouseOver)
                {
                    if (ImGui_IsCursorInScreen())
                    {
                        ImGui_UpdateCursorPosInScreenSpace();

                        auto front = yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldRotation().Forward();
                        auto distToXZPlane = abs(yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition().y);
                        auto projectedPos = yunutyEngine::graphics::Camera::GetMainCamera()->GetProjectedPoint({ cursorPos_InScreenSpace.first, cursorPos_InScreenSpace.second }, distToXZPlane, Vector3d(0, 1, 0));
                        pm->GetCurrentPalette()->OnMouseMove(projectedPos, { cursorPos_InScreenSpace.first, cursorPos_InScreenSpace.second });

                        if (eim.IsMouseButtonPressed(MouseCode::Left) && !ImGui_IsCursorInGizmoButtonRect())
                        {
                            if (Vector3d::Dot(projectedPos - yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition(), front) > 0)
                            {
                                pm->GetCurrentPalette()->OnLeftClick();
                            }
                        }

                        if (eim.IsMouseButtonUp(MouseCode::Left))
                        {
                            if (isGuizmoControl)
                            {
                                int idx = 0;
                                std::vector<std::tuple<IEditableData*, TransformData, TransformData>> vecList;
                                for (auto& each : pm->GetCurrentPalette()->GetSelections())
                                {
                                    each->GetPaletteInstance()->GetGameObject();
                                    vecList.push_back({ each, beforeTS[idx], each->GetPaletteInstance()->GetGameObject()->GetTransform() });
                                    idx++;
                                }
                                CommandManager::GetSingletonInstance().AddQueue(std::make_shared<TransformEditCommand>(vecList));
                            }
                            pm->GetCurrentPalette()->OnLeftClickRelease();
                            isGuizmoControl = false;
                        }
                    }
                    else
                    {
                        Release();
                    }
                }
                else
                {
                    Release();
                }

                // 키 입력에 대한 처리
                if (!eim.IsMouseButtonDown(MouseCode::Right))
                {
                    if (eim.IsKeyboardPressed(KeyCode::Q))
                    {
                        operation = (ImGuizmo::OPERATION)0;
                    }
                    else if (eim.IsKeyboardPressed(KeyCode::W))
                    {
                        operation = ImGuizmo::OPERATION::TRANSLATE;
                    }
                    else if (eim.IsKeyboardPressed(eim.GetKeyCode('E')))
                    {
                        operation = ImGuizmo::OPERATION::ROTATE;
                    }
                    else if (eim.IsKeyboardPressed(KeyCode::R))
                    {
                        operation = ImGuizmo::OPERATION::SCALE;
                    }
                }
            }
        }

        void SceneViewPanel::ImGui_OnResizeRenderImageSize()
        {
            // 그려지는 영역에 맞게 화면 비 재구성
            auto rect = yunutyEngine::graphics::Renderer::SingleInstance().GetResolution();
            float ratio = (float)rect.y / (float)rect.x;
            auto winMin = ImGui::GetWindowContentRegionMin();
            auto winMax = ImGui::GetWindowContentRegionMax();
            ImVec2 newRegion(winMax.x - winMin.x, winMax.y - winMin.y);
            float newRegionRatio = newRegion.y / newRegion.x;

            if (newRegionRatio >= ratio)
            {
                newRegion.y = newRegion.x * ratio;
            }
            else
            {
                newRegion.x = newRegion.y / ratio;
            }

            renderImageSize.first = newRegion.x;
            renderImageSize.second = newRegion.y;
        }

        void SceneViewPanel::ImGui_UpdateCursorPosInScreenSpace()
        {
            auto curPos = ImGui_GetCursorPosOnPanel();
            ImVec2 startPos = ImVec2(imageStartPos.first, imageStartPos.second);

            ImVec2 finalPos(curPos.first - startPos.x, curPos.second - startPos.y);

            cursorPos_InScreenSpace.first = (finalPos.x - (renderImageSize.first / 2)) / renderImageSize.first;
            cursorPos_InScreenSpace.second = ((renderImageSize.second / 2) - finalPos.y) / renderImageSize.second;
        }

        void SceneViewPanel::ImGui_UpdateWindowSize()
        {
            prevWindowSize.first = currentWindowSize.first;
            prevWindowSize.second = currentWindowSize.second;

            auto size = ImGui::GetContentRegionMax();
            currentWindowSize.first = size.x;
            currentWindowSize.second = size.y;

            auto pos = ImGui::GetCursorPos();
            imageStartPos.first = pos.x;
            imageStartPos.second = pos.y;
        }

        bool SceneViewPanel::ImGui_IsCursorInScreen()
        {
            auto curPos = ImGui_GetCursorPosOnPanel();
            ImVec2 startPos = ImVec2(imageStartPos.first, imageStartPos.second);
            return (curPos.first >= startPos.x && curPos.first <= startPos.x + renderImageSize.first) && (curPos.second >= startPos.y && curPos.second <= startPos.y + renderImageSize.second);
        }

        bool SceneViewPanel::ImGui_IsCursorInGizmoButtonRect()
        {
            auto curPos = ImGui_GetCursorPosOnPanel();
            ImVec2 startPos = ImVec2(imageStartPos.first + gizmoButtonStartPos.first, imageStartPos.second + gizmoButtonStartPos.second);
            return (curPos.first >= startPos.x - 5 && curPos.first <= startPos.x + gizmoButtonSize.first + 5) && (curPos.second >= startPos.y - 5 && curPos.second <= startPos.y + (gizmoButtonSize.second + 5) * 4 + 5);
        }

        bool SceneViewPanel::ImGui_IsWindowResize()
        {
            return (prevWindowSize != currentWindowSize);
        }

        std::pair<float, float> SceneViewPanel::ImGui_GetCursorPosOnPanel()
        {
            auto winPos = ImGui::GetWindowPos();
            auto curPos = ImGui::GetMousePos();

            return std::pair<float, float>(curPos.x - winPos.x, curPos.y - winPos.y);
        }

        void SceneViewPanel::ImGui_DrawGizmo()
        {
            // Gizmo Option Button
            {
                imgui::SmartStyleVar framePadding(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                imgui::SmartStyleVar itemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
                imgui::SmartStyleColor settingButtonBGColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

                auto selectedColor = ImVec4(1.0f, 0.3f, 0.1f, 1.0f);

                bool button[4] = {};

                switch (operation)
                {
                case ImGuizmo::TRANSLATE:
                    button[1] = true;
                    break;
                case ImGuizmo::ROTATE:
                    button[2] = true;
                    break;
                case ImGuizmo::SCALE:
                    button[3] = true;
                    break;
                default:
                    button[0] = true;
                    break;
                }

                auto palette = pm->GetCurrentPalette();
                auto buttonStartPos = ImVec2(imageStartPos.first + gizmoButtonStartPos.first, imageStartPos.second + gizmoButtonStartPos.second);

                ImGui::SetCursorPos(buttonStartPos);

                if (imgui::SelectableImageButton("Gizmo Select", "ImageButtons/Scene_Select.png", button[0], ImVec2(gizmoButtonSize.first, gizmoButtonSize.second), ImVec2(0, 0), ImVec2(1, 1), true, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), selectedColor))
                {
                    operation = (ImGuizmo::OPERATION)0;
                }

                ImGui::SetCursorPosX(buttonStartPos.x);

                if (imgui::SelectableImageButton("Gizmo Move", "ImageButtons/Scene_Move.png", button[1], ImVec2(gizmoButtonSize.first, gizmoButtonSize.second), ImVec2(0, 0), ImVec2(1, 1), true, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), selectedColor))
                {
                    operation = ImGuizmo::TRANSLATE;
                }

                ImGui::SetCursorPosX(buttonStartPos.x);

                if (imgui::SelectableImageButton("Gizmo Rotate", "ImageButtons/Scene_Rotate.png", button[2], ImVec2(gizmoButtonSize.first, gizmoButtonSize.second), ImVec2(0, 0), ImVec2(1, 1), true, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), selectedColor))
                {
                    operation = ImGuizmo::ROTATE;
                }

                ImGui::SetCursorPosX(buttonStartPos.x);

                if (imgui::SelectableImageButton("Gizmo Scale", "ImageButtons/Scene_Scale.png", button[3], ImVec2(gizmoButtonSize.first, gizmoButtonSize.second), ImVec2(0, 0), ImVec2(1, 1), true, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), selectedColor))
                {
                    operation = ImGuizmo::SCALE;
                }
            }

            ImGuizmo::SetDrawlist();
            float left = ImGui::GetWindowPos().x + imageStartPos.first;
            float top = ImGui::GetWindowPos().y + imageStartPos.second;
            float right = left + renderImageSize.first;
            float bottom = top + renderImageSize.second;
            ImGuizmo::SetRect(left, top, renderImageSize.first, renderImageSize.second);
            ImGui::PushClipRect(ImVec2(left, top), ImVec2(right, bottom), true);

            auto vtm = ec->GetVTM();
            auto ptm = ec->GetPTM();
            yunuGI::Matrix4x4 im = yunuGI::Matrix4x4();

            auto gvtm = math::ConvertVTM(vtm);
            auto gptm = math::ConvertPTM(ptm);

            auto beforeVTM = gvtm;

            ImGuizmo::ViewManipulate(reinterpret_cast<float*>(&gvtm), 10 * sqrt(3), ImVec2(ImGui::GetWindowPos().x + renderImageSize.first - 128, ImGui::GetWindowPos().y + imageStartPos.second), ImVec2(128, 128), 0x10101010);

            if (pm->GetCurrentPalette()->AreThereAnyObjectSelected())
            {
                auto& selections = pm->GetCurrentPalette()->GetSelections();
                yunuGI::Matrix4x4 tm = yunuGI::Matrix4x4();
                Vector3d startPosition = Vector3d();
                if (selections.size() == 1)
                {
                    if ((*selections.begin())->GetPaletteInstance())
                        tm = (*selections.begin())->GetPaletteInstance()->GetTransform()->GetWorldTM();
                }
                else
                {
                    for (auto each : selections)
                    {
                        startPosition += each->GetPaletteInstance()->GetTransform()->GetWorldPosition();
                    }
                    startPosition /= selections.size();
                    tm = math::GetTranslateMatrix(yunuGI::Vector3(startPosition.x, startPosition.y, startPosition.z));
                }

                auto objgwtm = math::ConvertWTM(tm);
                if (ImGuizmo::Manipulate(reinterpret_cast<float*>(&beforeVTM), reinterpret_cast<float*>(&gptm), operation, mode, reinterpret_cast<float*>(&objgwtm), NULL, NULL, NULL, NULL))
                {
                    if (!isGuizmoControl && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    {
                        OnStartControlGizmo();
                    }
                }

                if (selections.size() == 1)
                {
                    ImGui_UpdateEditableDataWTM(*selections.begin(), math::ConvertWTM(objgwtm));
                }
                else
                {
                    yunuGI::Vector3 scale = yunuGI::Vector3();
                    yunuGI::Quaternion rotate = yunuGI::Quaternion();
                    yunuGI::Vector3 trans = yunuGI::Vector3();

                    int idx = 0;

                    for (auto each : selections)
                    {
                        math::DecomposeWTM(math::ConvertWTM(objgwtm), scale, rotate, trans);
                        auto beforeScale = each->GetPaletteInstance()->GetTransform()->GetLocalScale();
                        auto beforeRotate = each->GetPaletteInstance()->GetTransform()->GetWorldRotation();
                        auto beforePos = each->GetPaletteInstance()->GetTransform()->GetWorldPosition();
                        if (isGuizmoControl)
                        {
                            beforeScale.x = initScale[idx].x * scale.x;
                            beforeScale.y = initScale[idx].y * scale.y;
                            beforeScale.z = initScale[idx].z * scale.z;
                        }
                        beforeRotate = beforeRotate * *reinterpret_cast<yunutyEngine::Quaternion*>(&rotate);
                        beforePos.x += (trans.x - startPosition.x);
                        beforePos.y += (trans.y - startPosition.y);
                        beforePos.z += (trans.z - startPosition.z);
                        each->OnRelocate(beforePos);
                        each->OnRerotate(beforeRotate);
                        each->OnRescale(beforeScale);
                        each->ApplyAsPaletteInstance();
                        idx++;
                    }
                }
            }

            if (ec->GetCameraPerspectiveState() == CameraPerspectiveState::Free)
            {
                if (beforeVTM[0][0] != gvtm[0][0] || beforeVTM[0][1] != gvtm[0][1] || beforeVTM[0][2] != gvtm[0][2] || beforeVTM[0][3] != gvtm[0][3] ||
                    beforeVTM[1][0] != gvtm[1][0] || beforeVTM[1][1] != gvtm[1][1] || beforeVTM[1][2] != gvtm[1][2] || beforeVTM[1][3] != gvtm[1][3] ||
                    beforeVTM[2][0] != gvtm[2][0] || beforeVTM[2][1] != gvtm[2][1] || beforeVTM[2][2] != gvtm[2][2] || beforeVTM[2][3] != gvtm[2][3] ||
                    beforeVTM[3][0] != gvtm[3][0] || beforeVTM[3][1] != gvtm[3][1] || beforeVTM[3][2] != gvtm[3][2] || beforeVTM[3][3] != gvtm[3][3])
                {
                    ec->ChangeVTM(math::ConvertVTM(gvtm));
                }
            }

            ImGui::PopClipRect();
        }

        void SceneViewPanel::Release()
        {
            if (pm->GetCurrentPalette())
                pm->GetCurrentPalette()->OnLeftClickRelease();
        }

        void SceneViewPanel::ImGui_SceneViewSettings()
        {
            imgui::SmartStyleVar disableSpacing(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            imgui::SmartStyleVar disableWindowBorder(ImGuiStyleVar_WindowBorderSize, 0.0f);
            imgui::SmartStyleVar windowRounding(ImGuiStyleVar_WindowRounding, 4.0f);
            imgui::SmartStyleVar disablePadding(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

            ImVec4 selectedColor = ImVec4(1.0f, 0.3f, 0.1f, 1.0f);

            ImVec2 buttonSize = ImVec2(20, 20);

            /// PlayMode
            ImVec2 pos1 = ImVec2(ImGui::GetWindowPos().x + currentWindowSize.first / 2 - buttonSize.x - 2, ImGui::GetWindowPos().y - 5);

            ImGui::SetNextWindowPos(pos1);
            ImGui::SetNextWindowSize(ImVec2(buttonSize.x * 2 + 4, buttonSize.y));
            ImGui::SetNextWindowBgAlpha(0.0f);

            ImGui::Begin("##PlayMode", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);
            static bool nowPlaying = false;
            static bool nowPausing = false;
            {
                {
                    imgui::SmartStyleVar framePadding(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    imgui::SmartStyleColor settingButtonBGColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

                    ImVec4 buttonColor = nowPlaying ? selectedColor : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

                    if (ImGui::ImageButton("Play", (ImTextureID)erm.GetTexture2D("ImageButtons/Play.png")->GetID(), buttonSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), buttonColor))
                    {
                        if (nowPlaying)
                        {
                            app->StopContents();
                            nowPlaying = false;
                            nowPausing = false;
                        }
                        else
                        {
                            if (!TerrainData::Instance().IsEmpty() && EditorCamera::GetSingletonInstance().GetCameraTypeState() == CameraTypeState::Editor)
                            {
                                app->PlayContents();
                                nowPlaying = true;
                            }
                        }
                    }
                }

                ImGui::SameLine();

                {
                    imgui::SmartStyleVar framePadding(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    imgui::SmartStyleColor settingButtonBGColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

                    ImVec4 buttonColor = nowPausing ? selectedColor : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

                    if (ImGui::ImageButton("Pause", (ImTextureID)erm.GetTexture2D("ImageButtons/Pause.png")->GetID(), buttonSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), buttonColor))
                    {
                        if (nowPausing)
                        {
                            app->PlayContents();
                            nowPausing = false;
                        }
                        else
                        {
                            if (nowPlaying)
                            {
                                app->PauseContents();
                                nowPausing = true;
                            }
                        }
                    }
                }
            }

            ImGui::End();

            /// Setting
            ImVec2 pos2 = ImVec2(ImGui::GetWindowPos().x + currentWindowSize.first - buttonSize.x, ImGui::GetWindowPos().y - 5);

            ImGui::SetNextWindowPos(pos2);
            ImGui::SetNextWindowSize(buttonSize);
            ImGui::SetNextWindowBgAlpha(0.0f);

            ImGui::Begin("##SceneView_Settings", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);

            bool openSettingsPopup = false;

            {
                {
                    imgui::SmartStyleVar framePadding(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    imgui::SmartStyleColor settingButtonBGColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

                    if (ImGui::ImageButton("SceneView Settings", (ImTextureID)erm.GetTexture2D("ImageButtons/Setting.png")->GetID(), buttonSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.0f, 0.0f, 0.0f, 0.0f)))
                    {
                        openSettingsPopup = true;
                    }
                }

                int sectionIdx = 0;

                static float popupWidth = 300.0f;

                imgui::SmartStyleVar itemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5.5f));
                imgui::SmartStyleVar windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
                imgui::SmartStyleVar cellPadding(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 5.5f));

                if (openSettingsPopup)
                {
                    ImGui::OpenPopup("SceneView Settings");
                }

                ImGui::SetNextWindowPos(ImVec2{ ImGui::GetWindowPos().x - popupWidth, ImGui::GetWindowPos().y + buttonSize.y });
                if (ImGui::BeginPopup("SceneView Settings", ImGuiWindowFlags_NoMove))
                {
                    if (imgui::BeginSection_2Col(sectionIdx, "EditorCamera", 300.0f))
                    {
                        CameraPerspectiveState state = ec->GetCameraPerspectiveState();
                        int selected = (int)state - 1;

                        static const char* selectionModes[] = { "Free", "Game" };
                        imgui::Dropdown_2Col("Perspective State", selectionModes, 2, (int32_t*)&selected);

                        switch ((CameraPerspectiveState)(selected + 1))
                        {
                        case application::editor::CameraPerspectiveState::Free:
                        {
                            ec->SetFreePerspective();
                            break;
                        }
                        case application::editor::CameraPerspectiveState::Game:
                        {
                            ec->SetGamePerspective();
                            break;
                        }
                        default:
                            break;
                        }
                        imgui::EndSection();
                    }

                    if (imgui::BeginSection_2Col(sectionIdx, "Environment", 300.0f))
                    {
                        auto lmState = wanderUtils::GetCurrentLightMap();
                        int selected = (int)lmState;

                        static const char* lightMapState[] = { "Stage 1", "Stage 2" };
                        imgui::Dropdown_2Col("Light Map", lightMapState, 2, (int32_t*)&selected);

                        wanderUtils::ChangeLightMap((wanderUtils::Stage)selected);

                        imgui::EndSection();
                    }

                    ImGui::EndPopup();
                }
            }

            ImGui::End();

        }

        void SceneViewPanel::ImGui_DrawMenuBar()
        {
            ImGui::BeginMenuBar();

            // View Mode
            if (ImGui::BeginMenu("View Mode"))
            {
                bool worldSelected = (mode == ImGuizmo::WORLD);

                if (ImGui::MenuItem("WORLD", 0, worldSelected))
                {
                    mode = ImGuizmo::WORLD;
                }
                if (ImGui::MenuItem("LOCAL", 0, !worldSelected))
                {
                    mode = ImGuizmo::LOCAL;
                }

                ImGui::EndMenu();
            }

            // Palette Type
            imgui::ShiftCursorX(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Interactable").x - ImGui::CalcTextSize(" | Select Mode").x - 10);

            switch (pm->GetCurrentPaletteType())
            {
            case palette::Palette_List::Terrain:
                ImGui::BeginMenu("     Terrain", false);
                break;
            case palette::Palette_List::Unit:
                ImGui::BeginMenu("        Unit", false);
                break;
            case palette::Palette_List::Ornament:
                ImGui::BeginMenu("    Ornament", false);
                break;
            case palette::Palette_List::Region:
                ImGui::BeginMenu("      Region", false);
                break;
            case palette::Palette_List::Wave:
                ImGui::BeginMenu("        Wave", false);
                break;
            case palette::Palette_List::Cam:
                ImGui::BeginMenu("         Cam", false);
                break;
            case palette::Palette_List::Light:
                ImGui::BeginMenu("       Light", false);
                break;
            case palette::Palette_List::Particle:
                ImGui::BeginMenu("    Particle", false);
                break;
            case palette::Palette_List::Interactable:
                ImGui::BeginMenu("Interactable", false);
                break;
            default:
                break;
            }

            // Palette Mode
            imgui::ShiftCursorX(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(" | Select Mode").x - 10);

            if (pm->GetCurrentPalette()->IsSelectMode())
            {
                ImGui::BeginMenu(" | Select Mode", false);
            }
            else
            {
                ImGui::BeginMenu(" | Place Mode", false);
            }

            ImGui::EndMenuBar();
        }

        void SceneViewPanel::ImGui_UpdateEditableDataWTM(IEditableData* target, const yunuGI::Matrix4x4& wtm) const
        {
            yunuGI::Vector3 scale;
            yunuGI::Quaternion rotation;
            yunuGI::Vector3 translation;
            math::DecomposeWTM(wtm, scale, rotation, translation);
            target->OnRescale(*reinterpret_cast<Vector3f*>(&scale));
            target->OnRerotate(*reinterpret_cast<Quaternion*>(&rotation));
            target->OnRelocate(*reinterpret_cast<Vector3f*>(&translation));
            target->ApplyAsPaletteInstance();
        }

        void SceneViewPanel::OnStartControlGizmo()
        {
            isGuizmoControl = true;
            initScale.resize(pm->GetCurrentPalette()->GetSelections().size());
            beforeTS.resize(initScale.size());
            int idx = 0;
            Vector3d tempScale = Vector3d();
            for (auto each : pm->GetCurrentPalette()->GetSelections())
            {
                tempScale = each->GetPaletteInstance()->GetTransform()->GetLocalScale();
                initScale[idx].x = tempScale.x;
                initScale[idx].y = tempScale.y;
                initScale[idx].z = tempScale.z;

                beforeTS[idx] = each->GetPaletteInstance()->GetTransform();

                idx++;
            }
        }
    }
}
