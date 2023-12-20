#include "EditorLayer.h"

#include "MapFileManager.h"
#include "EditorPanel.h"
#include "PanelList.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "MenubarCommands.h"
#include "Application.h"

namespace application
{
	namespace editor
	{
		std::function<void()> EditorLayer::testInitializer;

		void EditorLayer::Initialize()
		{
			/// Test 환경을 위한 Initialize 지원
			if (EditorLayer::testInitializer)
			{
				EditorLayer::testInitializer();
				return;
			}

			/// 각종 매니저 클래스 메모리 할당
			MapFileManager::GetInstance();

			/// 에디터에서 사용할 에디터 카메라, 게임뷰 카메라 생성

			/// 에디터 패널 생성 및 초기화 진행
			editorPanelList.resize((int)Panel_List::Size);

			editorPanelList[(int)Panel_List::HIERARCHY] = &HierarchyPanel::GetInstance();
			editorPanelList[(int)Panel_List::INSPECTOR] = &InspectorPanel::GetInstance();
			editorPanelList[(int)Panel_List::PREVIEW] = &PreviewPanel::GetInstance();
			editorPanelList[(int)Panel_List::MINIMAP] = &MiniMapPanel::GetInstance();
			editorPanelList[(int)Panel_List::SCENEVIEW] = &SceneViewPanel::GetInstance();
			editorPanelList[(int)Panel_List::CAMERAVIEW] = &CameraViewPanel::GetInstance();
			editorPanelList[(int)Panel_List::PALETTE] = &PalettePanel::GetInstance();

			for (auto each : editorPanelList)
			{
				each->Initialize();
			}
		}

		void EditorLayer::Update(float ts)
		{
			for (auto each : editorPanelList)
			{
				each->Update(ts);
			}
		}

		void EditorLayer::GUIProgress()
		{
			UI_DrawMenubar();

			for (auto each : editorPanelList)
			{
				each->GUIProgress();
			}
		}

		void EditorLayer::Finalize()
		{
			for (auto each : editorPanelList)
			{
				each->Finalize();
			}
		}

		void EditorLayer::AssignTestInitializer(std::function<void()> testInitializer)
		{
			EditorLayer::testInitializer = testInitializer;
		}

		/// private
		void EditorLayer::UI_DrawMenubar()
		{
			ImGui::BeginMenuBar();
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("LoadMap"))
				{
					cm.AddQueue(std::make_shared<LoadMapCommand>());
				}
				if (ImGui::MenuItem("SaveMap"))
				{
					cm.AddQueue(std::make_shared<SaveMapCommand>());
				}
				if (ImGui::MenuItem("Exit"))
				{
					cm.AddQueue(std::make_shared<ProgramExitCommand>());
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}
}
