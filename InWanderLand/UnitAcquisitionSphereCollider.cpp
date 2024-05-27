#include "UnitAcquisitionSphereCollider.h"
#include "InWanderLand.h"

void UnitAcquisitionSphereCollider::OnTriggerEnter(physics::Collider* other)
{
    if (auto unit = other->GetGameObject()->GetComponentWeakPtr<Unit>(); !unit.expired())
    {
        units.insert(unit);
        OnUnitEnter(unit);
        if (unit.lock()->GetTeamIndex() != owner.lock()->GetTeamIndex())
        {
            foes.insert(unit);
            OnHostileEnter(unit);
        }
        else
        {
            allies.insert(unit);
            OnAllyEnter(unit);
        }
    }
}

void UnitAcquisitionSphereCollider::OnTriggerExit(physics::Collider* other)
{
    if (auto unit = other->GetGameObject()->GetComponentWeakPtr<Unit>(); !unit.expired())
    {
        units.erase(unit);
        OnUnitExit(unit);
        if (unit.lock()->GetTeamIndex() != owner.lock()->GetTeamIndex())
        {
            foes.erase(unit);
            OnHostileExit(unit);
        }
        else
        {
            allies.erase(unit);
            OnAllyExit(unit);
        }
    }

}

void UnitAcquisitionSphereCollider::Update()
{
    static std::vector<std::weak_ptr<Unit>> foesToErase;
    static std::vector<std::weak_ptr<Unit>> alliesToErase;
    static std::vector<std::weak_ptr<Unit>> unitsToErase;

    foesToErase.clear();
    alliesToErase.clear();
    unitsToErase.clear();
    std::copy_if(foes.begin(), foes.end(), std::back_inserter(foesToErase), ShouldErase);
    std::copy_if(allies.begin(), allies.end(), std::back_inserter(alliesToErase), ShouldErase);
    std::copy_if(units.begin(), allies.end(), std::back_inserter(unitsToErase), ShouldErase);
    std::erase_if(foes, ShouldErase);
    std::erase_if(allies, ShouldErase);
    std::erase_if(units, ShouldErase);
    std::for_each(foesToErase.begin(), foesToErase.end(), [this](std::weak_ptr<Unit>& each) { OnHostileExit(each); });
    std::for_each(alliesToErase.begin(), alliesToErase.end(), [this](std::weak_ptr<Unit>& each) { OnAllyExit(each); });
    std::for_each(unitsToErase.begin(), unitsToErase.end(), [this](std::weak_ptr<Unit>& each) { OnUnitExit(each); });
}
bool UnitAcquisitionSphereCollider::ShouldErase(std::weak_ptr<Unit>& unit)
{
    return unit.expired() || (!includeDeadUnits && unit.lock()->GetCurrentUnitState() == UnitState::Death);
}
