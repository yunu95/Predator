#pragma once
#include "YunutyEngine.h"
#include "DebugGraphic.h"

class DebugStaticMesh :public graphics::StaticMeshRenderer, public DebugGraphic
{
public:
    DebugStaticMesh();
private:
};
