#include "YunutySphereCollider.h"
#include "_YunutySphereColliderImpl.h"

namespace yunutyEngine
{
    namespace physics
    {
        SphereCollider::SphereCollider() :Collider(impl = new SphereCollider::Impl{ this }) {}
        void SphereCollider::SetRadius(float radius)
        {
            impl->radius = radius;
        }
    }
}