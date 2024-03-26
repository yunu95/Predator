#include "InWanderLand.h"
#include "WanderUtils.h"

#include "EditorLayer.h"

#include "EditorPanel.h"
#include "PanelList.h"
#include "ModuleList.h"
#include "PaletteManager.h"
#include "Camera.h"
#include "DebugMeshes.h"
#include "EditorResourceManager.h"
#include "TemplateDataManager.h"
#include "InstanceManager.h"
#include "EditorCamera.h"
#include "EditorInputManager.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_Utility.h"

#include "MenubarCommands.h"
#include "Application.h"
#include "FileSystem.h"


bool editorInputControl = true;

namespace application
{
    namespace editor
    {
        std::function<void()> EditorLayer::testInitializer;

#ifdef GEN_TESTS
        void EditorLayer::AssignTestInitializer(std::function<void()> testInitializer)
        {
            EditorLayer::testInitializer = testInitializer;
        }
#endif

        void EditorLayer::SetInputControl(bool control)
        {
            editorInputControl = control;
        }

        bool EditorLayer::GetInputControl()
        {
            return editorInputControl;
        }

        void EditorLayer::Initialize()
        {
            /// Test 환경을 위한 Initialize 지원
            if (EditorLayer::testInitializer)
            {
                EditorLayer::testInitializer();
                return;
            }

            /// 에디터용 리소스 게임 엔진 리소스 매니저에 등록
            //LoadAllFBXFile();
            wanderUtils::LoadResourcesRecursively();

            /// 각종 매니저 클래스 메모리 할당
            MapFileManager::GetSingletonInstance();
            palette::PaletteManager::GetSingletonInstance();

            /// 에디터 패널 생성 및 초기화 진행
            editorPanelList.resize((int)Panel_List::Size);

            editorPanelList[(int)Panel_List::HIERARCHY] = &HierarchyPanel::GetSingletonInstance();
            editorPanelList[(int)Panel_List::INSPECTOR] = &InspectorPanel::GetSingletonInstance();
            editorPanelList[(int)Panel_List::PREVIEW] = &PreviewPanel::GetSingletonInstance();
            editorPanelList[(int)Panel_List::MINIMAP] = &MiniMapPanel::GetSingletonInstance();
            editorPanelList[(int)Panel_List::SCENEVIEW] = &SceneViewPanel::GetSingletonInstance();
            editorPanelList[(int)Panel_List::CAMERAVIEW] = &CameraViewPanel::GetSingletonInstance();
            editorPanelList[(int)Panel_List::PALETTE] = &PalettePanel::GetSingletonInstance();

            /// 에디터 모듈 생성 및 초기화 진행
            editorModuleList.resize((int)Module_List::Size);

            editorModuleList[(int)Module_List::GlobalConstant] = &Module_GlobalConstant::GetSingletonInstance();
            editorModuleList[(int)Module_List::TemplateDataEditor] = &Module_TemplateDataEditor::GetSingletonInstance();

            for (auto& each : editorPanelList)
            {
                each->Initialize();
            }

            for (auto& each : editorModuleList)
            {

                each->Initialize();
            }
        }

        void EditorLayer::Update(float ts)
        {
            editorCamera.Update(ts);

            for (auto& each : editorPanelList)
            {
                each->Update(ts);
            }

            for (auto& each : editorModuleList)
            {

                each->Update(ts);
            }

            /// Input 처리
            if (editorInputControl)
            {
                static auto& eim = EditorInputManager::GetSingletonInstance();

                /// Function
                if (eim.IsKeyboardPressed(KeyCode::F2))
                {
                    editorModuleList[(int)Module_List::GlobalConstant]->SetActivation(!editorModuleList[(int)Module_List::GlobalConstant]->GetActivation());
                }

                if (eim.IsKeyboardPressed(KeyCode::F3))
                {
                    editorModuleList[(int)Module_List::TemplateDataEditor]->SetActivation(!editorModuleList[(int)Module_List::TemplateDataEditor]->GetActivation());
                }

                /// Ctrl 조합
                if (eim.IsKeyboardDown(KeyCode::Control))
                {
                    if (eim.IsKeyboardPressed(KeyCode::S) && !mfm.GetCurrentMapPath().empty())
                    {
                        cm.AddQueue(std::make_shared<SaveMapCommand>());
                    }
                }
            }
        }

        void EditorLayer::GUIProgress()
        {
            UI_DrawMenubar();

            for (auto& each : editorPanelList)
            {
                each->GUIProgress();
            }

            for (auto& each : editorModuleList)
            {
                each->GUIProgress();
            }

            imgui::RenderMessageBoxes();
        }

        void EditorLayer::Finalize()
        {
            for (auto& each : editorPanelList)
            {
                each->Finalize();
            }

            for (auto& each : editorModuleList)
            {

                each->Finalize();
            }
        }

        void EditorLayer::OnEvent(EditorEvents& event)
        {
            editorCamera.OnEvent(event);

            for (auto& each : editorPanelList)
            {
                each->OnEvent(event);
            }

            for (auto& each : editorModuleList)
            {
                each->OnEvent(event);
            }
        }

        void EditorLayer::OnPlayContents()
        {
            for (auto& each : editorPanelList)
            {
                each->OnPlayContents();
            }

            for (auto& each : editorModuleList)
            {
                each->OnPlayContents();
            }

            for (auto each : InstanceManager::GetSingletonInstance().GetList())
            {
                auto ptr = each->GetPaletteInstance();
                if (ptr)
                {
                    ptr->HideEditorInstance();
                }
            }

            EditorCamera::GetSingletonInstance().OnPlayContents();
        }

        void EditorLayer::OnPauseContents()
        {
            for (auto& each : editorPanelList)
            {
                each->OnPauseContents();
            }

            for (auto& each : editorModuleList)
            {
                each->OnPauseContents();
            }

            for (auto each : InstanceManager::GetSingletonInstance().GetList())
            {
                auto ptr = each->GetPaletteInstance();
                if (ptr)
                {
                    ptr->ShowEditorInstance();
                }
            }

            EditorCamera::GetSingletonInstance().OnPauseContents();
        }

        void EditorLayer::OnResumeContents()
        {
            for (auto& each : editorPanelList)
            {
                each->OnResumeContents();
            }

            for (auto& each : editorModuleList)
            {
                each->OnResumeContents();
            }

            for (auto each : InstanceManager::GetSingletonInstance().GetList())
            {
                auto ptr = each->GetPaletteInstance();
                if (ptr)
                {
                    ptr->HideEditorInstance();
                }
            }

            EditorCamera::GetSingletonInstance().OnResumeContents();
        }

        void EditorLayer::OnStopContents()
        {
            for (auto& each : editorPanelList)
            {
                each->OnStopContents();
            }

            for (auto& each : editorModuleList)
            {
                each->OnStopContents();
            }

            for (auto each : InstanceManager::GetSingletonInstance().GetList())
            {
                auto ptr = each->GetPaletteInstance();
                if (ptr)
                {
                    ptr->ShowEditorInstance();
                }
            }

            EditorCamera::GetSingletonInstance().OnStopContents();
        }

        void EditorLayer::LateInitialize()
        {
            ResourceManager::GetSingletonInstance().LateInitialize();
            palette::PaletteManager::GetSingletonInstance().Initialize();
            palette::PaletteBrushManager::GetSingletonInstance().Initialize();
            mfm.LoadDefaultMap();

            // 카메라 초기화
            editorCamera.Initialize();

            // SceneGizmo
            InitSceneGizmo();

            for (auto& each : editorPanelList)
            {
                each->LateInitialize();
            }

            for (auto& each : editorModuleList)
            {
                each->LateInitialize();
            }
        }

        void EditorLayer::ReadyOrnament()
        {
            auto& templateDataManager = TemplateDataManager::GetSingletonInstance();
            for (auto& each : ResourceManager::GetSingletonInstance().GetStaticFBXList())
            {
                auto td = templateDataManager.CreateTemplateData<Ornament_TemplateData>(each);
                if (td)
                {
                    td->SetDataResourceName(each);
                }
            }
        }

        void EditorLayer::CreateDirectionalLight()
        {
            auto dl = TemplateDataManager::GetSingletonInstance().CreateTemplateData<Light_TemplateData>("Directional_Light");
            if (dl)
            {
                dl->pod.type = LightType::Directional;
                auto ld = InstanceManager::GetSingletonInstance().CreateInstance<LightData>("Directional_Light");
                auto rot = Quaternion{ Vector3d{90,0,30} };
                ld->pod.rotation.w = rot.w;
                ld->pod.rotation.x = rot.x;
                ld->pod.rotation.y = rot.y;
                ld->pod.rotation.z = rot.z;
                ld->pod.position.x = -999;
                ld->pod.position.y = 999;
                ld->pod.position.z = -999;
                ld->ApplyAsPaletteInstance();
                static_cast<PalettePanel*>(editorPanelList[(int)Panel_List::PALETTE])->ChangeDirectionalLight(ld);
            }
            else
            {
                for (auto each : InstanceManager::GetSingletonInstance().GetList<LightData>())
                {
                    if (each->pod.templateData->pod.type == LightType::Directional)
                    {
                        static_cast<PalettePanel*>(editorPanelList[(int)Panel_List::PALETTE])->ChangeDirectionalLight(each);
                        break;
                    }
                }
            }
        }

        void EditorLayer::LoadAllFBXFile()
        {
            // 재귀적으로 모든 FBX 로드하기
            const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

            auto directorList = fileSystem::GetSubdirectories("FBX");
            for (auto each : directorList)
            {
                resourceManager->LoadFile(("FBX/" + each.string()).c_str());
            }
        }

        void EditorLayer::InitSceneGizmo()
        {
            auto scene = yunutyEngine::Scene::getCurrentScene();
            if (scene)
            {
                // x축, z축 각각 그려질 grid 수(즉, 전체 그리드는 gridCount * 2)
                const int gridCount = 61;
                // 강조 될 단위
                const int accentUnit = 10;
                // 간격
                const int interval = 1;
                // 길이
                const int length = 60;

                for (unsigned int i = 0; i < gridCount; i++)
                {
                    int intervalPos = interval * i - ((gridCount - 1) / 2) * interval;
                    if (intervalPos == 0)
                    {
                        // x축
                        auto lineX = CreateLine(Vector3d(-length / 2, 0, 0), Vector3d(length / 2, 0, 0), yunuGI::Color::red());
                        lineX->SetIsUpdating(false);

                        // z축
                        auto lineZ = CreateLine(Vector3d(0, 0, -length / 2), Vector3d(0, 0, length / 2), yunuGI::Color::blue());
                        lineZ->SetIsUpdating(false);
                    }
                    else if (intervalPos % accentUnit == 0)
                    {
                        // x축 평행
                        auto lineX = CreateLine(Vector3d(-length / 2, 0, intervalPos), Vector3d(length / 2, 0, intervalPos), yunuGI::Color::white());
                        lineX->SetIsUpdating(false);

                        // z축 평행
                        auto lineZ = CreateLine(Vector3d(intervalPos, 0, -length / 2), Vector3d(intervalPos, 0, length / 2), yunuGI::Color::white());
                        lineZ->SetIsUpdating(false);
                    }
                    else
                    {
                        // x축 평행
                        auto lineX = CreateLine(Vector3d(-length / 2, 0, intervalPos), Vector3d(length / 2, 0, intervalPos), yunuGI::Color::gray());
                        lineX->SetIsUpdating(false);

                        // z축 평행
                        auto lineZ = CreateLine(Vector3d(intervalPos, 0, -length / 2), Vector3d(intervalPos, 0, length / 2), yunuGI::Color::gray());
                        lineZ->SetIsUpdating(false);
                    }
                }
            }
        }

        /// private
        void EditorLayer::UI_DrawMenubar()
        {
            ImGui::BeginMenuBar();
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("LoadUnrealData"))
                {
                    cm.AddQueue(std::make_shared<LoadStaticOrnamentsCommand>());
                }
                if (ImGui::MenuItem("SaveUnrealData"))
                {
                    cm.AddQueue(std::make_shared<SaveStaticOrnamentsCommand>());
                }
                if (ImGui::MenuItem("LoadMap"))
                {
                    cm.AddQueue(std::make_shared<LoadMapCommand>());
                }
                if (ImGui::MenuItem("SaveMap", "Ctrl + S", false, !mfm.GetCurrentMapPath().empty()))
                {
                    cm.AddQueue(std::make_shared<SaveMapCommand>());
                }
                if (ImGui::MenuItem("SaveMapAs"))
                {
                    cm.AddQueue(std::make_shared<SaveMapCommand>(true));
                }
                if (ImGui::MenuItem("Exit"))
                {
                    cm.AddQueue(std::make_shared<ProgramExitCommand>());
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Module"))
            {
                bool gc = editorModuleList[(int)Module_List::GlobalConstant]->GetActivation();
                if (ImGui::MenuItem("GlobalConstant Editor", "F2", gc))
                {
                    if (gc)
                    {
                        editorModuleList[(int)Module_List::GlobalConstant]->SetActivation(false);
                    }
                    else
                    {
                        editorModuleList[(int)Module_List::GlobalConstant]->SetActivation(true);
                    }
                }

                bool tde = editorModuleList[(int)Module_List::TemplateDataEditor]->GetActivation();
                if (ImGui::MenuItem("TemplateData Editor", "F3", tde))
                {
                    if (tde)
                    {
                        editorModuleList[(int)Module_List::TemplateDataEditor]->SetActivation(false);
                    }
                    else
                    {
                        editorModuleList[(int)Module_List::TemplateDataEditor]->SetActivation(true);
                    }
                }

                ImGui::EndMenu();
            }
            imgui::ShiftCursorX(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(mfm.GetCurrentMapPath().c_str()).x - 10);
            ImGui::Text(mfm.GetCurrentMapPath().c_str());
            ImGui::EndMenuBar();
        }
    }
}
