//#pragma once
//#include "RTSCam.h"
//#include "YunutyEngine.h"
//#include "DelayedTestFunctions.h"
//#include "ShakyCam.h"
//#include "DebugTilePlane.h"
//#include "DebugBeacon.h"
//#include "DebugMeshes.h"
//#include "YunutyEngine.h"
//
//void CreateNavPlane(Vector3f botleft, Vector3f topright, std::vector<Vector3f>& worldVertices, std::vector<int>& worldFaces)
//{
//    int startingIdx = worldVertices.size();
//    worldVertices.push_back({ botleft.x,0,topright.z });
//    worldVertices.push_back({ botleft.x,0,botleft.z });
//    worldVertices.push_back({ topright.x,0,botleft.z });
//    worldVertices.push_back({ topright.x,0,topright.z });
//
//    worldFaces.push_back(startingIdx + 2);
//    worldFaces.push_back(startingIdx + 1);
//    worldFaces.push_back(startingIdx + 0);
//    worldFaces.push_back(startingIdx + 3);
//    worldFaces.push_back(startingIdx + 2);
//    worldFaces.push_back(startingIdx + 0);
//
//    auto tilePlane = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DebugTilePlane>();
//    auto size = topright - botleft;
//    tilePlane->GetTransform()->SetWorldPosition((botleft + topright) / 2.0);
//    tilePlane->width = size.x;
//    tilePlane->height = size.z;
//    tilePlane->SetTiles();
//}
//
//NavigationAgent* CreateAgent(NavigationField* navField)
//{
//    auto agent = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationAgent>();
//    agent->SetSpeed(2);
//    agent->SetRadius(0.5);
//    agent->AssignToNavigationField(navField);
//    auto staticMesh = agent->GetGameObject()->AddGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
//    staticMesh->GetGI().SetMesh(graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(L"Capsule"));
//    staticMesh->GetGI().GetMaterial()->SetColor({ 0.75,0.75,0.75,0 });
//    staticMesh->GetTransform()->position = Vector3d{ 0,0.5,0 };
//    return agent;
//}
//// 길찾기 테스트
//void TestNavEnableDeath()
//{
//    const float corridorRadius = 3;
//    std::vector<Vector3f> worldVertices { };
//    std::vector<int> worldFaces { };
//
//    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
//    auto rtsCam = camObj->AddComponent<RTSCam>();
//    rtsCam->GetTransform()->position = Vector3d(0, 10, 0);
//
//    CreateNavPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
//    CreateNavPlane({ -8,0,-2 }, { 8,0,2 }, worldVertices, worldFaces);
//    CreateNavPlane({ -8,0,-8 }, { -6,0,8 }, worldVertices, worldFaces);
//    CreateNavPlane({ 6,0,-8 }, { 8,0,8 }, worldVertices, worldFaces);
//    CreateNavPlane({ -8,0,6 }, { 8,0,8 }, worldVertices, worldFaces);
//    CreateNavPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
//    auto navField = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationField>();
//    navField->BuildField(worldVertices, worldFaces);
//    auto agent = CreateAgent(navField);
//    auto agent2 = CreateAgent(navField);
//    auto agent3 = CreateAgent(navField);
//    auto delayedTestFunctions = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DelayedTestFunctions>();
//    delayedTestFunctions->todoList.push_back({ 3,[=]() {agent3->Relocate(Vector3d{3,0,0}); } });
//    delayedTestFunctions->todoList.push_back({ 8,[=]() {agent3->Relocate(Vector3d{3,9,8}); } });
//
//    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
//    directionalLight->GetTransform()->SetWorldRotation(Quaternion({100,10,0}));
//
//    agent3->Relocate(Vector3d{ 3,0,0 });
//    rtsCam->groundRightClickCallback = [=](Vector3d position)
//    {
//        agent->MoveTo(position);
//        agent2->MoveTo(position);
//        agent3->MoveTo(position);
//    };
//}
