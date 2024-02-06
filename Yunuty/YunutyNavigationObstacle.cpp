#include "YunutyNavigationField.h"
#include "YunutyNavigationObstacle.h"
#include "_YunutyNavigationObstacleImpl.h"
#include "_YunutyNavigationFieldImpl.h"

namespace yunutyEngine
{
    NavigationObstacle::NavigationObstacle()
    {
        impl = new Impl(this);
    }
    NavigationObstacle::~NavigationObstacle()
    {
        if (navField && (impl->obstacleRef != 0))
            navField->impl->DeleteObstacle(impl->obstacleRef);
        delete impl;
    }
    void NavigationObstacle::OnTransformUpdate()
    {
        ApplyObstacleState();
    }
    void NavigationObstacle::SetHalfExtents(const Vector3f& halfExtents)
    {
        impl->halfExtents = halfExtents;
        ApplyObstacleState();
    }
    void NavigationObstacle::ApplyObstacleState()
    {
        if (navField)
        {
            float yRadians = GetTransform()->GetWorldRotation().y * math::Deg2Rad;
            if (impl->obstacleRef != 0)
                navField->impl->DeleteObstacle(impl->obstacleRef);
            impl->obstacleRef = navField->impl->AddBoxObstacle(GetTransform()->GetWorldPosition(), impl->halfExtents, yRadians);
        }
    }
    void NavigationObstacle::AssignToNavigationField(NavigationField* navField)
    {
        this->navField = navField;
        ApplyObstacleState();
    }
}
