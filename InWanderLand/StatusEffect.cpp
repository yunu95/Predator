#include "InWanderLand.h"
#include "StatusEffect.h"
#include "Unit.h"
#include "ContentsLayer.h"
#include "Application.h"

std::weak_ptr<physics::SphereCollider> StatusEffect::GetTriggerCollider()
{
    if (triggerCollider.expired())
    {
        triggerCollider = GetGameObject()->GetComponentWeakPtr<physics::SphereCollider>();
    }
    return triggerCollider;
}
void StatusEffect::Start()
{
}

void StatusEffect::SetSkillOwnerUnit(Unit* p_unit)
{
    m_ownerUnit = p_unit;
}

