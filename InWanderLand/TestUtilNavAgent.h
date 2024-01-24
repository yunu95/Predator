#pragma once
#include "YunutyEngine.h"

namespace tests
{
    NavigationAgent* CreateNavAgent(const Vector3d& pos,NavigationField* navField)
    {
        auto agent = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationAgent>();
        agent->GetTransform()->position = pos;
        agent->SetSpeed(2);
        agent->SetRadius(0.5);
        agent->AssignToNavigationField(navField);
        auto staticMesh = agent->GetGameObject()->AddGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        staticMesh->GetGI().SetMesh(graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(L"Capsule"));
        staticMesh->GetGI().GetMaterial()->SetColor({ 0.75,0.75,0.75,1 });
        staticMesh->GetTransform()->position = Vector3d{ 0,0.5,0 };
        return agent;
    }
}
