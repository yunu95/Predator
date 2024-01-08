#include "YunutyBoxCollider.h"
#include "_YunutyBoxColliderImpl.h"

using namespace std;
namespace yunutyEngine
{
    namespace physics
    {
        BoxCollider::BoxCollider() : Collider(impl = new BoxCollider::Impl{ this })
        {
        }
        void BoxCollider::SetHalfExtent(const Vector3f& halfExtent)
        {
            unscaledHalfExtent = halfExtent;
            ApplyScale(GetTransform()->GetWorldScale());
        }
        void BoxCollider::ApplyScale(const Vector3d& scale)
        {
            Vector3f halfExtent{ static_cast<float>(unscaledHalfExtent.x * scale.x),static_cast<float>(unscaledHalfExtent.y * scale.y),static_cast<float>(unscaledHalfExtent.z * scale.z) };
            impl->halfExtent = halfExtent;
            impl->ResetGeometry();
        }
    }
}
