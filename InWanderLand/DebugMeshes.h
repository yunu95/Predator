#pragma once
#include "YunutyEngine.h"

using namespace yunutyEngine;

enum class DebugMeshType
{
    None,
    Cube,
    Sphere,
    Rectangle,
};

yunuGI::IMaterial* GetColoredDebugMaterial(yunuGI::Color color, bool isWireFrame);
inline void CreateLine(Vector3d start, Vector3d end, yunuGI::Color color = yunuGI::Color::red(), bool isWireFrame = true, float cylinderRadius = 0.1f)
{
    auto gameObject = Scene::getCurrentScene()->AddGameObject();
    auto staticMesh = gameObject->AddComponent<graphics::StaticMeshRenderer>();
    staticMesh->GetGI().GetMaterial()->SetColor(color);
    ///staticMesh->GetGI().GetMaterial()->SetPixelShader(isWireFrame ? L"DebugPS.cso" : L"DefaultPS.cso");
    ///staticMesh->GetGI().SetMesh(L"Cylinder");
}
inline yunutyEngine::graphics::StaticMeshRenderer* AttachDebugMesh(GameObject* target, DebugMeshType meshType = DebugMeshType::Cube, yunuGI::Color color = yunuGI::Color::red(), bool isWireFrame = true)
{
    auto staticMesh = target->AddComponent<graphics::StaticMeshRenderer>();
    wstring meshName;
    switch (meshType)
    {
    case DebugMeshType::Cube:
        meshName = L"Cube";
        break;
    case DebugMeshType::Sphere:
        meshName = L"Sphere";
        break;
    case DebugMeshType::Rectangle:
        meshName = L"Rectangle";
        break;
    }
    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    staticMesh->GetGI().SetMesh(rsrcManager->GetMesh(meshName));
    staticMesh->GetGI().SetMaterial(0, GetColoredDebugMaterial(color, isWireFrame));
    return staticMesh;
}
