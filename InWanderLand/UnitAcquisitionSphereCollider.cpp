#include "UnitAcquisitionSphereCollider.h"
#include "InWanderLand.h"

void UnitAcquisitionSphereCollider::OnTriggerEnter(physics::Collider* other)
{
    if (auto unit = other->GetGameObject()->GetComponent<Unit>(); unit)
    {
        units.insert(unit);
        OnUnitEnter(unit);
        int teamIndex = GetTeamIndex();
        int otherTeamIndex = unit->GetTeamIndex();
        if (teamIndex != otherTeamIndex)
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
    if (!other) return;
    if (auto unit = other->GetGameObject()->GetComponent<Unit>(); unit)
    {
        units.erase(unit);
        OnUnitExit(unit);
        if (unit->GetTeamIndex() != GetTeamIndex())
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
    static std::vector<Unit*> foesToErase;
    static std::vector<Unit*> alliesToErase;
    static std::vector<Unit*> unitsToErase;
    foesToErase.clear();
    alliesToErase.clear();
    unitsToErase.clear();
    auto ShouldErase = [this](Unit* const unit) {
        return (!includeDeadUnits && !unit->IsAlive()) || (!includeInvulnerableUnits && unit->IsInvulenerable());
        };
    std::copy_if(units.begin(), units.end(), std::back_inserter(unitsToErase), ShouldErase);
    std::copy_if(allies.begin(), allies.end(), std::back_inserter(alliesToErase), ShouldErase);
    std::copy_if(foes.begin(), foes.end(), std::back_inserter(foesToErase), ShouldErase);
    std::erase_if(foes, ShouldErase);
    std::erase_if(allies, ShouldErase);
    std::erase_if(units, ShouldErase);
    std::for_each(foesToErase.begin(), foesToErase.end(), [this](Unit* each) { OnHostileExit(each); });
    std::for_each(alliesToErase.begin(), alliesToErase.end(), [this](Unit* each) { OnAllyExit(each); });
    std::for_each(unitsToErase.begin(), unitsToErase.end(), [this](Unit* each) { OnUnitExit(each); });
}

int UnitAcquisitionSphereCollider::GetTeamIndex()
{
    if (owner.expired())
        return teamIndex;
    else
        return owner.lock()->GetTeamIndex();
}
