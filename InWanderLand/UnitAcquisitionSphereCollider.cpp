#include "UnitAcquisitionSphereCollider.h"
#include "InWanderLand.h"

void UnitAcquisitionSphereCollider::OnTriggerEnter(physics::Collider* other)
{
    if (auto unit = other->GetGameObject()->GetComponentWeakPtr<Unit>(); !unit.expired())
    {
        if (unit.lock()->GetTeamIndex() != owner.lock()->GetTeamIndex())
        {
            foes.insert(unit);
        }
        else
        {
            allies.insert(unit);
        }
    }
}

void UnitAcquisitionSphereCollider::OnTriggerExit(physics::Collider* other)
{
    if (auto unit = other->GetGameObject()->GetComponentWeakPtr<Unit>(); !unit.expired())
    {
        if (unit.lock()->GetTeamIndex() != owner.lock()->GetTeamIndex())
        {
            foes.erase(unit);
        }
        else
        {
            allies.erase(unit);
        }
    }

}
