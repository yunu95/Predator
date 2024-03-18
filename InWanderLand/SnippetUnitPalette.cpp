#include "InWanderLand.h"
#if defined(GEN_TESTS) && defined(EDITOR)
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "RTSCam.h"
#include "TestUtilRTSTestCam.h"
#include "SelectionBox.h"
#include "PaletteManager.h"
#include "TerrainPalette.h"
#include "UnitPalette.h"
#include "UnitEditorInstance.h"
#include "Unit_TemplateData.h"
#include "TemplateDataManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace application::editor::palette;



namespace snippets
{
    namespace editor_palette
    {
        TEST_CLASS(SnippetUnitPaletteClass)
        {
        public:
            // 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
            static void SnippetInitializerUnitPalette()
            {
                using namespace application::editor::palette;
                using namespace application::editor;
                yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

                auto& app = application::Application::GetInstance();

                auto defaultUnitTemplate = TemplateDataManager::GetSingletonInstance().CreateTemplateData<Unit_TemplateData>("DefaultUnit");
                UnitPalette::SingleInstance().SelectUnitTemplateData(defaultUnitTemplate);

                auto rtsCam = Scene::getCurrentScene()->AddGameObject()->AddComponent<tests::RTSTestCam>();
                rtsCam->GetTransform()->SetLocalPosition(Vector3d(3, 10, 3));
                application::editor::palette::PaletteManager::GetSingletonInstance().SetCurrentPalette(&application::editor::palette::UnitPalette::SingleInstance());
                rtsCam->groundHoveringClickCallback = [&](const Vector3d& worldPos) { if (application::Application::IsFocusGameWindow()) { application::editor::palette::UnitPalette::SingleInstance().OnMouseMove(worldPos, Vector2d{ 0,0 }); } };
                rtsCam->groundLeftClickCallback = [&](const Vector3d& worldPos) { application::editor::palette::UnitPalette::SingleInstance().OnLeftClick(); };
                rtsCam->groundLeftClickReleaseCallback = [&](const Vector3d& worldPos) { if (application::Application::IsFocusGameWindow()) { application::editor::palette::UnitPalette::SingleInstance().OnLeftClickRelease(); } };
                rtsCam->buttonCallbackDelete = [&]() { if (application::Application::IsFocusGameWindow()) { application::editor::palette::UnitPalette::SingleInstance().OnDeletion(); } };
                rtsCam->buttonCallback_X = [&]() { UnitPalette::SingleInstance().SelectUnitTemplateData(TemplateDataManager::GetSingletonInstance().GetTemplateData<Unit_TemplateData>("DefaultUnit")); };
                rtsCam->buttonCallbackSpace = [&]()
                {
                    if (application::Application::IsFocusGameWindow())
                    {
                        UnitPalette::SingleInstance().SetAsSelectMode(!UnitPalette::SingleInstance().IsSelectMode());
                    }
                };



                auto directionalLight = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::DirectionalLight>();

                //auto tempUnit1 = Scene::getCurrentScene()->AddGameObject()->AddComponent<UnitEditorInstance>();
                //tempUnit1->GetTransform()->position = Vector3d(0, 0, 0);
                //auto tempUnit2 = Scene::getCurrentScene()->AddGameObject()->AddComponent<UnitEditorInstance>();
                //tempUnit2->GetTransform()->position = Vector3d(3, 0, 0);
                //auto tempUnit3 = Scene::getCurrentScene()->AddGameObject()->AddComponent<UnitEditorInstance>();
                //tempUnit3->GetTransform()->position = Vector3d(0, 0, 3);

                SelectionBox::Instance();
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
}
#endif
