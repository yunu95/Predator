#pragma once
#include "Vector3.h"
#include "Component.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    class NavigationField;
    class YUNUTY_API NavigationObstacle : virtual public Component
    {
    public:
        enum class ObstacleType
        {
            Cylinder,
            Box
        };
        class Impl;
        NavigationObstacle();
        virtual ~NavigationObstacle();
        virtual void OnTransformUpdate()override;
        virtual void OnEnable() override;
        virtual void OnDisable() override;
        void AssignToNavigationField(NavigationField* navField);
        void SetHalfExtents(const Vector3f& halfExtents);
        void SetRadiusAndHeight(float radius, float height);
        void SetObstacleType(ObstacleType obstacleType);
        ObstacleType GetObstacleType();
    private:
        ObstacleType obstacleType{ ObstacleType::Box };
        void ApplyObstacleState();
        Impl* impl;
        NavigationField* navField{ nullptr };
        friend NavigationField;
    };
}
