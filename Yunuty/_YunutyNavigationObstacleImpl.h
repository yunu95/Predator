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
        NavigationObstacle* owner{ nullptr };
        dtObstacleRef obstacleRef{ 0 };
        Vector3f halfExtents;
        float radius;
        float height;
    };
}
