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
#include "SingleNavigationField.h"
#include "TerrainInstance.h"
#include "TestUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace application::editor::palette;


namespace snippets
{
    void SnippetInitializerTerrainPalette()
    {
        yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
        auto rtsCam = Scene::getCurrentScene()->AddGameObject()->AddComponent<tests::RTSTestCam>();
        auto buttonEvent = Scene::getCurrentScene()->AddGameObject()->AddComponent<tests::ButtonEvent>();
        static vector<NavigationAgent*> agents;

        rtsCam->GetTransform()->position = Vector3d(3, 10, 3);
        PaletteManager::SetCurrentPalette(&TerrainPaletteManager::SingleInstance());
        rtsCam->groundHoveringClickCallback = [](const Vector3d& worldPos) {PaletteManager::GetCurrentPalette()->OnMouseMove(worldPos); };
        rtsCam->groundLeftClickCallback = [](const Vector3d& worldPos) {
            if (Input::isKeyDown(KeyCode::Control) && SingleNavigationField::Instance().IsInitialized())
                agents.push_back(tests::CreateNavAgent(worldPos, &SingleNavigationField::Instance()));
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
        buttonEvent->keyPushEvents[KeyCode::C] = []() {TerrainPaletteManager::SingleInstance().SetIsMarking(!TerrainPaletteManager::SingleInstance().IsMarking()); };
        buttonEvent->keyPushEvents[KeyCode::B] = [=]() {
            PaletteManager::GetCurrentPalette()->ApplyAsPlaytimeObjects();
            for (auto each : agents)
            {
                each->AssignToNavigationField(&SingleNavigationField::Instance());
            }
        };
        TerrainPaletteManager::SingleInstance().SetBrushSize(2);
        TerrainPaletteManager::GetCurrentPalette()->SetAsSelectMode(false);

        auto directionalLight = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::DirectionalLight>();
        directionalLight->GetTransform()->rotation = Vector3d(45, 45, 0);

        SelectionBox::Instance();
        yunutyEngine::YunutyCycle::SingleInstance().Play();
    }
    TEST_CLASS(SnippetTerrainPaletteClass)
    {
    public:
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
#endif
