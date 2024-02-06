#pragma once
#include "Recast.h"
#include "_YunutyNavigationFieldImpl.h"

namespace yunutyEngine
{
    class NavigationObstacle::Impl
    {
    public:
        Impl(NavigationObstacle* owner)
        {
            this->owner = owner;
        };
        ~Impl()
        {};
        NavigationObstacle* owner;
        dtObstacleRef obstacleRef;
        Vector3f halfExtents;
    };
}
