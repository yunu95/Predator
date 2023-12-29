#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "RTSCam.h"
#include "TerrainPaletteManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetInitializerEditTerrain()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    auto rtsCam = Scene::getCurrentScene()->AddGameObject()->AddComponent<RTSCam>();


    rtsCam->GetTransform()->position = Vector3d(3, 10, 3);
    application::editor::palette::PaletteManager::SetCurrentPalette(&application::editor::palette::TerrainPaletteManager::SingleInstance());
    rtsCam->groundHoveringClickCallback = [](const Vector3d& worldPos) {application::editor::palette::TerrainPaletteManager::GetCurrentPalette()->OnMouseMove(worldPos); };
    rtsCam->groundLeftClickCallback = [](const Vector3d& worldPos) {application::editor::palette::TerrainPaletteManager::GetCurrentPalette()->OnLeftClick(); };
    rtsCam->groundLeftClickReleaseCallback = [](const Vector3d& worldPos) {application::editor::palette::TerrainPaletteManager::GetCurrentPalette()->OnLeftClickRelease(); };
    yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{
    TEST_CLASS(InWanderLand)
    {
    public:
        // 테스트 함수의 이름이 Snippet으로 시작하는 테스트들은 빌드의 성공 여부 판단에 쓰이지 않습니다.
        // RunTests.bat를 실행해도 이 테스트들은 실행되지 않으며, Jenkins에서도 이 테스트들은 실행되지 않습니다.
        // 이 테스트들은 오직 개발자가 직접 엔트리 포인트를 달리하여 테스트를 진행하고 싶을 때의 용도로만 사용됩니다.
        TEST_METHOD(SnippetEditTerrain)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::Contents::ContentsLayer::AssignTestInitializer(SnippetInitializerEditTerrain);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif