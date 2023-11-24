#include "ContentsLayer.h"

#include "YunutyEngine.h"
#include "ShakyCam.h"
#include "RTSCam.h"   
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "DebugMeshes.h"

#include <d3d11.h>

void CreateNavPlane(Vector3f botleft, Vector3f topright, std::vector<Vector3f>& worldVertices, std::vector<int>& worldFaces)
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
    staticMesh->GetGI().LoadMesh("Capsule");
    staticMesh->GetGI().GetMaterial()->SetColor({ 0.75,0.75,0.75,0 });
    staticMesh->GetTransform()->position = Vector3d{ 0,0.5,0 };
    return agent;
}
void Application::Contents::ContentsLayer::Initialize()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    yunutyEngine::Collider2D::SetIsOnXYPlane(false);

    //auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //camObj->GetTransform()->position = Vector3d(0, 0, -5);
    //auto roamingCam = camObj->AddComponent<RoamingCam>();

    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

    auto rtsCam = camObj->AddComponent<RTSCam>();
    rtsCam->GetTransform()->position = Vector3d(0, 10, 0);

    // phsyx collider 테스트
    {
        auto boxCollider = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto boxColliderComp = boxCollider->AddComponent<yunutyEngine::physics::BoxCollider>();
        auto rigidComp = boxCollider->AddComponent<physics::RigidBody>();
        //rigidComp->SetAsStatic(true);
        boxCollider->GetTransform()->position = Vector3d(0, 10, 0);
        boxCollider->GetTransform()->rotation = Vector3d(100, 10, 35);
        auto boxMesh = boxCollider->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        boxMesh->GetGI().LoadMesh("Cube");
        boxMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::black());
        //rigidComp->LockTranslation(1,1,1);
        rigidComp->SetAsKinematic(true);
        //rigidComp->AddForce({ 0, -10, 0 }, physics::ForceType::VELOCITY_CHANGE);

        auto planeCollider = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto planeColliderComp = planeCollider->AddComponent<yunutyEngine::physics::BoxCollider>();
        planeColliderComp->SetHalfExtent({ 10,1,10 });
        //auto planeRigidComp = planeCollider->AddComponent<physics::RigidBody>();
        //planeRigidComp->SetAsStatic(true);
        planeCollider->GetTransform()->position = Vector3d(0, 0, 0);
        auto planeMesh = planeCollider->AddGameObject();
        auto planeMeshComp = planeMesh->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        planeMeshComp->GetGI().LoadMesh("Cube");
        planeMeshComp->GetGI().GetMaterial()->SetColor(yunuGI::Color::white());
        planeMesh->GetTransform()->scale = Vector3d(10, 1, 10);
    }


    // 길찾기 테스트
    {
        const float corridorRadius = 3;
        std::vector<Vector3f> worldVertices { };
        std::vector<int> worldFaces { };

        CreateNavPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
        CreateNavPlane({ -8,0,-2 }, { 8,0,2 }, worldVertices, worldFaces);
        CreateNavPlane({ -8,0,-8 }, { -6,0,8 }, worldVertices, worldFaces);
        CreateNavPlane({ 6,0,-8 }, { 8,0,8 }, worldVertices, worldFaces);
        CreateNavPlane({ -8,0,6 }, { 8,0,8 }, worldVertices, worldFaces);
        CreateNavPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
        auto navField = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationField>();
        navField->BuildField(worldVertices, worldFaces);
        auto agent = CreateAgent(navField);
        auto agent2 = CreateAgent(navField);
        auto agent3 = CreateAgent(navField);
        rtsCam->groundRightClickCallback = [=](Vector3d position)
        {
            agent->MoveTo(position);
            agent2->MoveTo(position);
            agent3->MoveTo(position);
        };
    }

    yunutyEngine::YunutyCycle::SingleInstance().Play();
}

void Application::Contents::ContentsLayer::Update(float ts)
{

}

void Application::Contents::ContentsLayer::GUIProgress()
{

}

void Application::Contents::ContentsLayer::Finalize()
{

}
