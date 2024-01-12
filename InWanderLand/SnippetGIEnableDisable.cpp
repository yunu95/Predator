#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "DebugMeshes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;



void SnippetInitializerGIEnableDisable()
{
    class DebuggingMesh : public yunutyEngine::Component
    {
    public:
        float t{0};
    private:
        graphics::StaticMeshRenderer* debugMesh;
    protected:
        virtual void Start() override
        {
            debugMesh = AttachDebugMesh(GetGameObject(), DebugMeshType::Cube, yunuGI::Color::green(), false);
        }
        virtual void Update() override
        {
            t += Time::GetDeltaTime();
            GetGameObject()->GetTransform()->position = { 3 * sin(t * 2), 0, 0 };
            debugMesh->SetActive(static_cast<int>(t) % 2 == 0);
        }

    };
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    auto cam = Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::Camera>();
    cam->GetTransform()->position = { 0,0,-10 };
    auto debugMesh = Scene::getCurrentScene()->AddGameObject()->AddComponent<DebuggingMesh>();

    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 10,10,0 }));
    yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace tests
{
    TEST_CLASS(Snippets)
    {
    public:
        /// <summary>
        /// SnippetGIEnableDisable 코드는 그래픽스 객체가 껐다 다시 키면 트랜스폼이 반영되지 않는 괴현상을 테스트합니다.
        /// </summary>
        TEST_METHOD(SnippetGIEnableDisable)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::Contents::ContentsLayer::AssignTestInitializer(SnippetInitializerGIEnableDisable);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif
