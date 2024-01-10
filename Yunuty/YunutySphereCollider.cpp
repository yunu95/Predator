#include "YunutySphereCollider.h"
#include "_YunutySphereColliderImpl.h"

namespace yunutyEngine
{
    namespace physics
    {
        SphereCollider::SphereCollider() :Collider(impl = new SphereCollider::Impl{ this }) {}
        void SphereCollider::SetRadius(float radius)
        {
            unscaledRadius = radius;
            ApplyScale(GetTransform()->GetWorldScale());
        }
        void SphereCollider::ApplyScale(const Vector3d& scale)
        {
            impl->radius = unscaledRadius * static_cast<float>(scale.x);
            impl->ResetGeometry();
        }
    }
}