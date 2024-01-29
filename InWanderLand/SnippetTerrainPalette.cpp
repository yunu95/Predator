#include "InWanderLand.h"
#ifdef GEN_TESTS && defined(EDITOR)
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "RTSCam.h"
#include "SelectionBox.h"
#include "PaletteManager.h"
#include "TerrainPalette.h"
#include "SingleNavigationField.h"
#include "TerrainInstance.h"
#include "TestUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace application::editor::palette;


namespace snippets
{
	namespace editor_palette
	{
		// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
		void SnippetInitializerTerrainPalette()
		{
			yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
			auto rtsCam = Scene::getCurrentScene()->AddGameObject()->AddComponent<tests::RTSTestCam>();
			rtsCam->GetTransform()->position = Vector3d(3, 10, 3);
			rtsCam->GetTransform()->SetWorldRotation(Vector3d(60, 0, 0));
			static vector<NavigationAgent*> agents;

			//auto buttonEvent = Scene::getCurrentScene()->AddGameObject()->AddComponent<tests::ButtonEvent>();
			/*PaletteManager::GetSingletonInstance().SetCurrentPalette(&TerrainPalette::SingleInstance());
			rtsCam->groundHoveringClickCallback = [](const Vector3d& worldPos) {PaletteManager::GetCurrentPalette()->OnMouseMove(worldPos); };
			rtsCam->groundLeftClickCallback = [](const Vector3d& worldPos) {
				if (Input::isKeyDown(KeyCode::Control) && SingleNavigationField::Instance().IsInitialized())
					agents.push_back(CreateNavAgent(worldPos, &SingleNavigationField::Instance()));
				else
					PaletteManager::GetCurrentPalette()->OnLeftClick();
				};
			rtsCam->groundRightClickCallback = [](const Vector3d& worldPos) {
				if (SingleNavigationField::Instance().IsInitialized())
				{
					for (auto each : agents)
					{
						each->MoveTo(worldPos);
					}
				}
				};
			rtsCam->groundLeftClickReleaseCallback = [](const Vector3d& worldPos) {PaletteManager::GetCurrentPalette()->OnLeftClickRelease(); };
			buttonEvent->keyPushEvents[KeyCode::Delete] = []() {PaletteManager::GetCurrentPalette()->OnDeletion(); };
			buttonEvent->keyPushEvents[KeyCode::C] = []() {
				application::editor::palette::PaletteManager::GetCurrentPalette()->SetAsSelectMode(false);
				TerrainPaletteManager::SingleInstance().SetIsMarking(!TerrainPaletteManager::SingleInstance().IsMarking());
				};
			buttonEvent->keyPushEvents[KeyCode::B] = [=]() {
				PaletteManager::GetCurrentPalette()->ApplyAsPlaytimeObjects();
				for (auto each : agents)
				{
					each->AssignToNavigationField(&SingleNavigationField::Instance());
				}
				};
			buttonEvent->keyPushEvents[KeyCode::P] = [=]() {
				auto palette = application::editor::palette::PaletteManager::GetCurrentPalette();
				palette->SetAsSelectMode(true);
				};


			TerrainPaletteManager::SingleInstance().SetBrushSize(2);
			TerrainPaletteManager::GetCurrentPalette()->SetAsSelectMode(false);*/

			auto directionalLight = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::DirectionalLight>();
			directionalLight->GetTransform()->rotation = Vector3d(45, 45, 0);

			SelectionBox::Instance();
			yunutyEngine::YunutyCycle::SingleInstance().Play();
		}
		TEST_CLASS(SnippetTerrainPaletteClass)
		{
		public:
			// 테스트 함수의 이름이 Snippet으로 시작하는 테스트들은 빌드의 성공 여부 판단에 쓰이지 않습니다.
			// RunTests.bat를 실행해도 이 테스트들은 실행되지 않으며, Jenkins에서도 이 테스트들은 실행되지 않습니다.
			// 이 테스트들은 오직 개발자가 직접 엔트리 포인트를 달리하여 테스트를 진행하고 싶을 때의 용도로만 사용됩니다.
			TEST_METHOD(SnippetTerrainPalette)
			{
				application::Application& client = application::Application::CreateApplication(0, 0);
				application::contents::ContentsLayer::AssignTestInitializer(SnippetInitializerTerrainPalette);
				client.Initialize();
				client.Run();
				client.Finalize();
			}
		};
	}
}
#endif
