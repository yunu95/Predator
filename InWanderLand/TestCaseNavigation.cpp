#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "RTSCam.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "ShakyCam.h"
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "DebugMeshes.h"
#include "YunutyEngine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void CreateNavigationPlane(Vector3f botleft, Vector3f topright, std::vector<Vector3f>& worldVertices, std::vector<int>& worldFaces)
{
    int startingIdx = worldVertices.size();
    worldVertices.push_back({ botleft.x,0,topright.z });
    worldVertices.push_back({ botleft.x,0,botleft.z });
    worldVertices.push_back({ topright.x,0,botleft.z });
    worldVertices.push_back({ topright.x,0,topright.z });

    worldFaces.push_back(startingIdx + 2);
    worldFaces.push_back(startingIdx + 1);
    worldFaces.push_back(startingIdx + 0);
    worldFaces.push_back(startingIdx + 3);
    worldFaces.push_back(startingIdx + 2);
    worldFaces.push_back(startingIdx + 0);

    auto tilePlane = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DebugTilePlane>();
    auto size = topright - botleft;
    tilePlane->GetTransform()->SetWorldPosition((botleft + topright) / 2.0);
    tilePlane->width = size.x;
    tilePlane->height = size.z;
    tilePlane->SetTiles();
}

NavigationAgent* CreateAgent(NavigationField* navField)
{
    auto agent = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationAgent>();
    agent->SetSpeed(2);
    agent->SetRadius(0.5);
    agent->AssignToNavigationField(navField);
    auto staticMesh = agent->GetGameObject()->AddGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    staticMesh->GetGI().SetMesh(graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(L"Capsule"));
    staticMesh->GetGI().GetMaterial()->SetColor({ 0.75,0.75,0.75,1 });
    staticMesh->GetTransform()->SetLocalPosition(Vector3d{ 0,0.5,0 });
    return agent;
}
// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void TestCaseNavigationInit()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

    const float corridorRadius = 3;
    std::vector<Vector3f> worldVertices{ };
    std::vector<int> worldFaces{ };

    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto rtsCam = camObj->AddComponent<graphics::Camera>();
    rtsCam->SetCameraMain();
    rtsCam->GetTransform()->SetLocalPosition( Vector3d(3, 10, 3));
    rtsCam->GetTransform()->SetWorldRotation({ Vector3d{90,0,0} });

    CreateNavigationPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
    CreateNavigationPlane({ -8,0,-2 }, { 8,0,2 }, worldVertices, worldFaces);
    CreateNavigationPlane({ -8,0,-8 }, { -6,0,8 }, worldVertices, worldFaces);
    CreateNavigationPlane({ 6,0,-8 }, { 8,0,8 }, worldVertices, worldFaces);
    CreateNavigationPlane({ -8,0,6 }, { 8,0,8 }, worldVertices, worldFaces);
    CreateNavigationPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
    auto navField = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationField>();
    navField->BuildField(worldVertices, worldFaces);
    auto agent = CreateAgent(navField);
    auto delayedTestFunctions = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DelayedTestFunctions>();

    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));

    //rtsCam->groundRightClickCallback = [=](Vector3d position)
    //{
    //    agent->MoveTo(position);
    //};
    const Vector3d moveDestination{ 6.5,0,6.5 };
    agent->MoveTo(moveDestination);
    agent->SetSpeed(10);
    agent->SetAcceleration(10000000);
    // delayedTestFunctions에 2초 후 실행시킬 콜백 함수를 등록합니다. 이 콜백함수는 게임 엔진 스레드에서 호출됩니다.
    delayedTestFunctions->todoList.push_back({ 2,[=]() {
        // 게임 엔진 스레드에서 메인 스레드에서 특정 동작을 구동시키고 싶다면 아래의 AddMainLoopTodo 함수를 사용합니다.
        application::Application::GetInstance().AddMainLoopTodo([=]() {
            // Assert 함수군은 테스트 케이스의 실행 성공 여부를 판단하는데에 쓰입니다.
            // Assert의 실행은 메인 스레드에서 실행되어야 합니다.
            Assert::IsTrue((agent->GetTransform()->GetWorldPosition() - moveDestination).MagnitudeSqr() < 0.3,L"navigation agent couldn't move to a specific location!");
            agent->Relocate(Vector3d{ -10,0,0 });
                });
            } });
    delayedTestFunctions->todoList.push_back({ 2.2,[=]() {
        // 게임 엔진 스레드에서 메인 스레드에서 특정 동작을 구동시키고 싶다면 아래의 AddMainLoopTodo 함수를 사용합니다.
        application::Application::GetInstance().AddMainLoopTodo([=]() {
            Assert::IsTrue((agent->GetTransform()->GetWorldPosition() - Vector3d{-7.5,0,0}).MagnitudeSqr() < 1,L"navigation agent's ""Relocate"" method didn't really relocate the agent!");
            // 위 식이 참이라면 프로그램을 종료합니다. 
            application::Application::GetInstance().TurnOff();
                });
            } });
    yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace tests
{
    TEST_CLASS(TestCaseNavigationClass)
    {
    public:
        TEST_METHOD(TestCaseNavigation)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::contents::ContentsLayer::AssignTestInitializer(TestCaseNavigationInit);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif
