#pragma once
#include "YunutyEngine.h"
#include "DebugStaticMesh.h"

using namespace yunutyEngine;

enum class DebugMeshType
{
    None,
    Line,
    Cube,
    Sphere,
    Rectangle,
};

yunuGI::IMaterial* GetColoredDebugMaterial(yunuGI::Color color, bool isWireFrame);

inline yunutyEngine::graphics::StaticMeshRenderer* AttachDebugMesh(GameObject* target, DebugMeshType meshType = DebugMeshType::Cube, yunuGI::Color color = yunuGI::Color::red(), bool isWireFrame = true)
{
    auto staticMesh = target->AddComponent<DebugStaticMesh>();
    wstring meshName;
    switch (meshType)
    {
    case DebugMeshType::Line:
        // Line은 기본 트랜스폼으로 배치되었을 시 0,0,0을 시점으로, 1,0,0을 종점으로 한다.
        meshName = L"Line";
        break;
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

inline yunutyEngine::graphics::StaticMeshRenderer* CreateLine(Vector3d start, Vector3d end, yunuGI::Color color = yunuGI::Color::red())
{
    if (start == end)
    {
        return nullptr;
    }

    auto gameObject = Scene::getCurrentScene()->AddGameObject();
    auto delta = (end - start);
    gameObject->GetTransform()->SetLocalScale({ delta.Magnitude(), 1, 1 });

    Vector3d axis = Vector3d::Cross(Vector3d(1, 0, 0), delta).Normalized();
    if (axis == Vector3d(0, 0, 0))
    {
        axis = Vector3d(0, 0, 1);
    }

    gameObject->GetTransform()->SetWorldRotation(Quaternion::MakeAxisAngleQuaternion(axis, acos(Vector3d::Dot(Vector3d(1, 0, 0), delta.Normalized()))));
    gameObject->GetTransform()->SetWorldPosition(start);
    return AttachDebugMesh(gameObject, DebugMeshType::Line, color, true);
}

