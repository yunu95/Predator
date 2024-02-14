#pragma once
#include "_YunutyColliderImpl.h"
#include "YunutySphereCollider.h"

using namespace std;
namespace yunutyEngine
{
    namespace physics
    {
        class SphereCollider::Impl : public Collider::Impl
        {
        public:
            Impl(Collider* collider) : Collider::Impl(collider) {}
            float radius{ 0 };
            PxSphereGeometry geometry;
            virtual PxGeometry& GetGeometry()override
            {
                radius = abs(radius);
                geometry = PxSphereGeometry{ radius };
                return geometry;
            }
        };
    }
}
