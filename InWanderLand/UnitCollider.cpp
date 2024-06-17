#include "InWanderLand.h"
#include "UnitCollider.h"

Unit* UnitCollider::AcquireUnit(physics::Collider* collider)
{
    if (collider == nullptr)
        return nullptr;
    if (auto unitCollider = collider->GetGameObject()->GetComponent<UnitCollider>(); unitCollider)
    {
        return unitCollider->owner.lock().get();
    }
    return nullptr;
}
