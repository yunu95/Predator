#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "DebugMeshes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace snippets
{
    TEST_CLASS(SnippetCamPosAndNearClass)
    {
        static void SnippetInitializerCamPosAndNear()
        {
            class EpsilonVibration : public yunutyEngine::Component
            {
            protected:
                double epsilon = 0.001;
                virtual void Start() {
                    GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() - Vector3d::forward * epsilon * 0.5);
                }
                virtual void Update() {
                    GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + Vector3d::forward * epsilon);
                    epsilon *= -1;
                }
            };
            yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
            auto xyPlane = AttachDebugMesh(yunutyEngine::Scene::getCurrentScene()->AddGameObject(), DebugMeshType::Rectangle, yunuGI::Color::blue(), false);
            auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
            xyPlane->GetGameObject()->AddComponent<EpsilonVibration>();
            directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));

            auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::Camera>();
            camObj->GetTransform()->SetWorldPosition({ 0,0,-5 });
            xyPlane->GetTransform()->SetWorldPosition({ 0,0,-5 + 2 });
            camObj->GetGI().SetNear(2);

        }
    public:
        /// <summary>
        /// 이 스니펫 코드는 카메라의 위치가 near plane이 아니라 카메라 사영의 소실점에 위치한다는 것을 증명하는 코드입니다.
        /// xyPlane은 camObj로부터 near거리만큼 떨어진 곳에서 z축 좌표를 미세하게 바꾸며 진동합니다.
        /// 만약 xyPlane이 계속해서 사라졌다가 나타나기를 반복한다면 이는 카메라의 위치가 카메라 사영의 소실점에 위치하는 것을 증명합니다.
        /// </summary>
        TEST_METHOD(SnippetCamPosAndNear)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::contents::ContentsLayer::AssignTestInitializer(SnippetInitializerCamPosAndNear);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
};
#endif
