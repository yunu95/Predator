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
#include "TestUtils.h"

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
    auto rtsCam = camObj->AddComponent<tests::RTSTestCam>();
    rtsCam->SetCameraMain();
    rtsCam->GetTransform()->SetLocalPosition(Vector3d(3, 20, 3));
    rtsCam->GetTransform()->SetWorldRotation({ Vector3d{90,0,0} });

    CreateNavigationPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
    CreateNavigationPlane({ -8,0,-2 }, { 8,0,2 }, worldVertices, worldFaces);
    CreateNavigationPlane({ -8,0,-8 }, { -6,0,8 }, worldVertices, worldFaces);
    CreateNavigationPlane({ 6,0,-8 }, { 8,0,8 }, worldVertices, worldFaces);
    CreateNavigationPlane({ -8,0,6 }, { 8,0,8 }, worldVertices, worldFaces);
    CreateNavigationPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
    auto navField = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationField>();
    navField->BuildField(worldVertices, worldFaces);

    // 장애물 객체를 추가하는 모습
    GameObject* obstacleTobeRemoved;
    {
        auto obstacle = Scene::getCurrentScene()->AddGameObject();
        AttachDebugMesh(obstacle);
        obstacle->GetTransform()->SetLocalScale({ 2,10,2 });
        obstacle->GetTransform()->SetWorldPosition({ -5,5,7 });
        auto obstacleComp = obstacle->AddComponent<yunutyEngine::NavigationObstacle>();
        obstacleComp->SetHalfExtents({ 1,5,1 });
        obstacleComp->AssignToNavigationField(navField);
        obstacleTobeRemoved = obstacle;
    }
    {
        auto obstacle = Scene::getCurrentScene()->AddGameObject();
        AttachDebugMesh(obstacle);
        obstacle->GetTransform()->SetLocalScale({ 2,10,2 });
        obstacle->GetTransform()->SetWorldPosition({ -7,5,5 });
        auto obstacleComp = obstacle->AddComponent<yunutyEngine::NavigationObstacle>();
        obstacleComp->SetHalfExtents({ 1,5,1 });
        obstacleComp->AssignToNavigationField(navField);
    }


    auto agent = CreateAgent(navField);
    auto agentStuck = CreateAgent(navField);
    auto delayedTestFunctions = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DelayedTestFunctions>();

    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));

    rtsCam->groundRightClickCallback = [=](Vector3d position)
    {
        agent->MoveTo(position);
    };
    rtsCam->buttonCallback_X = [=]()
    {
        application::editor::EditorCamera::GetSingletonInstance().SwitchCam();
    };
    const Vector3d moveDestination{ 6.5,0,6.5 };
    agent->MoveTo(moveDestination);
    agent->SetSpeed(10);
    agentStuck->SetSpeed(10);
    agent->SetAcceleration(30);
    agentStuck->Relocate(Vector3d{ -7,0,7 });
    agentStuck->MoveTo(Vector3d{ 0,0,0 });
    delayedTestFunctions->todoList.push_back({ 1.0,[=]() {
        // 게임 엔진 스레드에서 메인 스레드에서 특정 동작을 구동시키고 싶다면 아래의 AddMainLoopTodo 함수를 사용합니다.
        application::Application::GetInstance().AddMainLoopTodo([=]() {
            // Assert 함수군은 테스트 케이스의 실행 성공 여부를 판단하는데에 쓰입니다.
            // Assert의 실행은 메인 스레드에서 실행되어야 합니다.
            // 장애물에 가로막힌 agentStuck은 왼쪽 위 모서리에 갇혀져 있을 것이다. 라는 가설을 검증
            Assert::IsTrue(agentStuck->GetTransform()->GetWorldPosition().x < -6,L"navigation agent couldn't move to a specific location!");
                });
       Scene::getCurrentScene()->DestroyGameObject(obstacleTobeRemoved);
       agentStuck->MoveTo(Vector3d{ 0,0,0 });
            } });
    // delayedTestFunctions에 2초 후 실행시킬 콜백 함수를 등록합니다. 이 콜백함수는 게임 엔진 스레드에서 호출됩니다.
    delayedTestFunctions->todoList.push_back({ 5.2,[=]() {
        // 게임 엔진 스레드에서 메인 스레드에서 특정 동작을 구동시키고 싶다면 아래의 AddMainLoopTodo 함수를 사용합니다.
        application::Application::GetInstance().AddMainLoopTodo([=]() {
            // Assert 함수군은 테스트 케이스의 실행 성공 여부를 판단하는데에 쓰입니다.
            // Assert의 실행은 메인 스레드에서 실행되어야 합니다.
            // "장애물에 가로막힌 agentStuck은 왼쪽 위 모서리를 빠져 나왔을 것이다." 라는 가설을 검증
            //Assert::IsTrue(agentStuck->GetTransform()->GetWorldPosition().x > -6,L"navigation agent couldn't move to a specific location!");
            Assert::IsTrue((agent->GetTransform()->GetWorldPosition() - moveDestination).MagnitudeSqr() < 0.3,L"navigation agent couldn't move to a specific location!");
            application::Application::GetInstance().TurnOff();
                });
            } });
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
