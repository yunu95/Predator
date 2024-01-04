#include "YunutyCapsuleCollider.h"
#include "_YunutyCapsuleColliderImpl.h"

namespace yunutyEngine
{
    namespace physics
    {
        CapsuleCollider::CapsuleCollider() :Collider(impl = new CapsuleCollider::Impl{ this }) {}
        void CapsuleCollider::SetRadius(float radius)
        {
            unscaledRadius = radius;
            ApplyScale(GetTransform()->GetWorldScale());
        }
        void CapsuleCollider::SetHalfHeight(float halfHeight)
        {
            unscaledHalfHeight = halfHeight;
            ApplyScale(GetTransform()->GetWorldScale());
        }
        void CapsuleCollider::ApplyScale(const Vector3d& scale)
        {
            impl->radius = unscaledRadius * static_cast<float>(scale.x);
            impl->halfHeight = unscaledHalfHeight * static_cast<float>(scale.y);
            impl->ResetGeometry();
        }
    }
}
