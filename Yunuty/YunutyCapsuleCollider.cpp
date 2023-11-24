#include "YunutyCapsuleCollider.h"
#include "_YunutyCapsuleColliderImpl.h"

namespace yunutyEngine
{
    namespace physics
    {
        CapsuleCollider::CapsuleCollider() :Collider(impl = new CapsuleCollider::Impl{ this }) {}
        void CapsuleCollider::SetRadius(float radius)
        {
            impl->radius = radius;
        }
        void CapsuleCollider::SetHalfHeight(float halfHeight)
        {
            impl->halfHeight = halfHeight;
        }
    }
}
