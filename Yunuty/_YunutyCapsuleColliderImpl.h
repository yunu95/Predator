#pragma once
#include "_YunutyColliderImpl.h"
#include "YunutyCapsuleCollider.h"

using namespace std;
namespace yunutyEngine
{
    namespace physics
    {
        class CapsuleCollider::Impl : public Collider::Impl
        {
        public:
            Impl(Collider* collider) : Collider::Impl(collider) {}
            float halfHeight{ 0 };
            float radius{ 0 };
            PxCapsuleGeometry geometry;
            virtual PxGeometry& GetGeometry()override
            {
                //assert(radius != 0);
                geometry = PxCapsuleGeometry{ radius,halfHeight };
                return geometry;
            }
        };
    }
}

