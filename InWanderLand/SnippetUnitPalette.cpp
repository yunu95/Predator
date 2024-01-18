#include "InWanderLand.h"
#if defined(GEN_TESTS) && defined(EDITOR)
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "RTSCam.h"
#include "SelectionBox.h"
#include "TerrainPaletteManager.h"
#include "UnitPaletteManager.h"
#include "UnitInstance.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace application::editor::palette;



namespace snippets
{
    TEST_CLASS(SnippetUnitPaletteClass)
    {
    public:
        // 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
        static void SnippetInitializerUnitPalette()
        {
            yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
            auto rtsCam = Scene::getCurrentScene()->AddGameObject()->AddComponent<RTSCam>();

            auto& app = application::Application::GetInstance();

            rtsCam->GetTransform()->position = Vector3d(3, 10, 3);
            application::editor::palette::PaletteManager::SetCurrentPalette(&application::editor::palette::UnitPaletteManager::SingleInstance());
            rtsCam->groundHoveringClickCallback = [&](const Vector3d& worldPos) { if (application::Application::IsFocusGameWindow()){ application::editor::palette::UnitPaletteManager::GetCurrentPalette()->OnMouseMove(worldPos); } };
            rtsCam->groundLeftClickCallback = [&](const Vector3d& worldPos) { application::editor::palette::UnitPaletteManager::GetCurrentPalette()->OnLeftClick(); };
            rtsCam->groundLeftClickReleaseCallback = [&](const Vector3d& worldPos) { if (application::Application::IsFocusGameWindow()) { application::editor::palette::UnitPaletteManager::GetCurrentPalette()->OnLeftClickRelease(); } };
            rtsCam->deleteButtonCallback = [&]() { if (application::Application::IsFocusGameWindow()) { application::editor::palette::PaletteManager::GetCurrentPalette()->OnDeletion(); } };
            rtsCam->xButtonCallback = [&]()
            {
                if (application::Application::IsFocusGameWindow())
                {
                    auto palette = application::editor::palette::PaletteManager::GetCurrentPalette();
                    palette->SetAsSelectMode(!palette->IsSelectMode());
                }
            };

            auto directionalLight = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::DirectionalLight>();

            auto tempUnit1 = Scene::getCurrentScene()->AddGameObject()->AddComponent<UnitInstance>();
            tempUnit1->GetTransform()->position = Vector3d(0, 0, 0);
            auto tempUnit2 = Scene::getCurrentScene()->AddGameObject()->AddComponent<UnitInstance>();
            tempUnit2->GetTransform()->position = Vector3d(3, 0, 0);
            auto tempUnit3 = Scene::getCurrentScene()->AddGameObject()->AddComponent<UnitInstance>();
            tempUnit3->GetTransform()->position = Vector3d(0, 0, 3);

            SelectionBox::Instance();
            yunutyEngine::YunutyCycle::SingleInstance().Play();
        }
        // 테스트 함수의 이름이 Snippet으로 시작하는 테스트들은 빌드의 성공 여부 판단에 쓰이지 않습니다.
        // RunTests.bat를 실행해도 이 테스트들은 실행되지 않으며, Jenkins에서도 이 테스트들은 실행되지 않습니다.
        // 이 테스트들은 오직 개발자가 직접 엔트리 포인트를 달리하여 테스트를 진행하고 싶을 때의 용도로만 사용됩니다.
        TEST_METHOD(SnippetUnitPalette)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::contents::ContentsLayer::AssignTestInitializer(SnippetInitializerUnitPalette);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif
