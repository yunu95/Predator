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
        if (GetActive())
            ApplyObstacleState();
    }
    void NavigationObstacle::SetHalfExtents(const Vector3f& halfExtents)
    {
        impl->halfExtents = halfExtents;
        ApplyObstacleState();
    }
    void NavigationObstacle::SetRadiusAndHeight(float radius, float height)
    {
        impl->radius = radius;
        impl->height = height;
        ApplyObstacleState();
    }
    void NavigationObstacle::SetObstacleType(ObstacleType obstacleType)
    {
        this->obstacleType = obstacleType;
    }
    NavigationObstacle::ObstacleType NavigationObstacle::GetObstacleType()
    {
        return obstacleType;
    }
    void NavigationObstacle::ApplyObstacleState()
    {
        if (navField)
        {
            float yRadians = GetTransform()->GetWorldRotation().y * math::Deg2Rad;
            if (impl->obstacleRef != 0)
                navField->impl->DeleteObstacle(impl->obstacleRef);
            switch (obstacleType)
            {
            case ObstacleType::Box:
                impl->obstacleRef = navField->impl->AddBoxObstacle(GetTransform()->GetWorldPosition(), impl->halfExtents, yRadians);
                break;
            case ObstacleType::Cylinder:
                impl->obstacleRef = navField->impl->AddCylinderObstacle(GetTransform()->GetWorldPosition(), impl->radius, impl->height);
                break;
            default:
                assert(false);
                break;
            }
        }
    }
    void NavigationObstacle::OnEnable()
    {
        ApplyObstacleState();
    }
    void NavigationObstacle::OnDisable()
    {
        if (navField && impl->obstacleRef != 0)
        {
            navField->impl->DeleteObstacle(impl->obstacleRef);
            impl->obstacleRef = 0;
        }
    }
    void NavigationObstacle::AssignToNavigationField(NavigationField* navField)
    {
        if (this->navField != nullptr)
        {
            if (impl->obstacleRef != 0)
                this->navField->impl->DeleteObstacle(impl->obstacleRef);
            this->navField->obstacles.erase(this);
        }
        if (navField != nullptr)
        {
            navField->obstacles.insert(this);
        }
        this->navField = navField;

        ApplyObstacleState();
    }
}
