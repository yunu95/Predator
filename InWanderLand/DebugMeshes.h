#pragma once
#include "YunutyEngine.h"
//#include "DebugStaticMesh.h"

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
yunuGI::IMaterial* GetColoredDebugMaterialTransparent(yunuGI::Color color);

inline yunutyEngine::graphics::StaticMeshRenderer* AttachDebugMesh(GameObject* target, DebugMeshType meshType = DebugMeshType::Cube, yunuGI::Color color = yunuGI::Color::red(), bool isWireFrame = true);
inline yunutyEngine::graphics::StaticMeshRenderer* CreateLine(Vector3d start, Vector3d end, yunuGI::Color color = yunuGI::Color::red());