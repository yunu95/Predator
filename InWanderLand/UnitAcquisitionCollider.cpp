#include "UnitAcquisitionCollider.h"
#include "InWanderLand.h"

bool UnitAcquisitionCollider::ShouldContain(Unit* unit)
{
    return (includeDeadUnits || unit->IsAlive()) && (includeInvulnerableUnits || !unit->IsInvulenerable());
}

void UnitAcquisitionCollider::OnTriggerEnter(physics::Collider* other)
{
    if (auto collider = other->GetGameObject()->GetComponent<UnitCollider>(); collider)
    {
        auto unit = collider->owner.lock().get();
        unitsWhatSoEver.insert(unit);
        bool includeDeadUnits = false;
        bool includeInvulnerableUnits = false;
        if (ShouldContain(unit))
        {
            units.insert(unit);
            int teamIndex = GetTeamIndex();
            int otherTeamIndex = unit->GetTeamIndex();
            if (teamIndex != otherTeamIndex)
            {
                enemies.insert(unit);
            }
            else
            {
                friends.insert(unit);
            }
        }
    }
}

void UnitAcquisitionCollider::OnTriggerExit(physics::Collider* other)
{
    if (!other) return;
    if (auto collider = other->GetGameObject()->GetComponent<UnitCollider>(); collider)
    {
        auto unit = collider->owner.lock().get();
        unitsWhatSoEver.erase(unit);
        units.erase(unit);
        enemies.erase(unit);
        friends.erase(unit);
    }
}

void UnitAcquisitionCollider::Update()
{
    units.clear();
    friends.clear();
    enemies.clear();
    std::copy_if(unitsWhatSoEver.begin(), unitsWhatSoEver.end(), std::inserter(units, units.end()), [this](Unit* const unit) { return ShouldContain(unit); });
    std::copy_if(units.begin(), units.end(), std::inserter(friends, friends.end()), [this](Unit* const unit) {return unit->GetTeamIndex() == GetTeamIndex(); });
    std::copy_if(units.begin(), units.end(), std::inserter(enemies, enemies.end()), [this](Unit* const unit) {return unit->GetTeamIndex() != GetTeamIndex(); });
}

int UnitAcquisitionCollider::GetTeamIndex()
{
    if (owner.expired())
        return teamIndex;
    else
        return owner.lock()->GetTeamIndex();
}