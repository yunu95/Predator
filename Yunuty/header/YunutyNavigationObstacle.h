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
    class YUNUTY_API NavigationObstacle : public Component
    {
    public:
        class Impl;
        NavigationObstacle();
        virtual ~NavigationObstacle();
        virtual void OnTransformUpdate()override;
        void AssignToNavigationField(NavigationField* navField);
        void SetHalfExtents(const Vector3f& halfExtents);
    private:
        void ApplyObstacleState();
        Impl* impl;
        NavigationField* navField{ nullptr };
        friend NavigationField;
    };
}
