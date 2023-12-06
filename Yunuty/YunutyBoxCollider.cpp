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
            impl->halfExtent = halfExtent;
        }
    }
}
