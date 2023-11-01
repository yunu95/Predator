#pragma once
#include "YunutyEngine.h"

using namespace yunutyEngine;

inline void CreateLine(Vector3d start, Vector3d end, yunuGI::Color color = yunuGI::Color::red(), bool isWireFrame = true, float cylinderRadius = 0.1f)
{
    auto gameObject = Scene::getCurrentScene()->AddGameObject();
    auto staticMesh = gameObject->AddComponent<graphics::StaticMeshRenderer>();
    staticMesh->GetGI().GetMaterial()->SetColor(color);
    staticMesh->GetGI().GetMaterial()->SetPixelShader(isWireFrame ? L"DebugPS.cso" : L"DefaultPS.cso");
    staticMesh->GetGI().SetMesh(L"Cylinder");
}
