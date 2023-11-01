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
    class YUNUTY_API NavigationAgent : public Component
    {
    public:
        class Impl;
        NavigationAgent();
        virtual ~NavigationAgent();
        virtual void Update();
        void AssignToNavigationField(NavigationField* navField);
        void SetSpeed(float speed);
        void SetAcceleration(float accel);
        void SetRadius(float radius);
        const Vector3f& GetTargetPosition();
        float GetSpeed();
        float GetAcceleration();
        float GetRadius();
        void MoveTo(Vector3f destination);
        void MoveTo(Vector3d destination) { MoveTo(Vector3f{ destination }); }
    private:
        Impl* impl;
        NavigationField* navField;
        friend NavigationField;
    };
}
