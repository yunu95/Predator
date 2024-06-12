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
            enemies.insert(unit);
            OnEnemyEnter(unit);
        }
        else
        {
            friends.insert(unit);
            OnFriendEnter(unit);
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
            enemies.erase(unit);
            OnEnemyExit(unit);
        }
        else
        {
            friends.erase(unit);
            OnFriendExit(unit);
        }
    }

}

UnitAcquisitionSphereCollider::UnitAcquisitionSphereCollider()
    : SphereCollider()
{
    debugMesh = GetGameObject()->AddGameObject();
    AttachDebugMesh(debugMesh, DebugMeshType::Sphere, yunuGI::Color::green());
}

void UnitAcquisitionSphereCollider::SetRadius(float radius)
{
    physics::SphereCollider::SetRadius(radius);
    debugMesh->GetTransform()->SetWorldScale(2 * radius * Vector3d::one);
}

void UnitAcquisitionSphereCollider::SetColor(yunuGI::Color color)
{
    auto dm = debugMesh->GetComponent<DebugStaticMesh>();
    dm->GetGI().GetMaterial()->SetColor(color);
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
    std::copy_if(friends.begin(), friends.end(), std::back_inserter(alliesToErase), ShouldErase);
    std::copy_if(enemies.begin(), enemies.end(), std::back_inserter(foesToErase), ShouldErase);
    std::erase_if(enemies, ShouldErase);
    std::erase_if(friends, ShouldErase);
    std::erase_if(units, ShouldErase);
    std::for_each(foesToErase.begin(), foesToErase.end(), [this](Unit* each) { OnEnemyExit(each); });
    std::for_each(alliesToErase.begin(), alliesToErase.end(), [this](Unit* each) { OnFriendExit(each); });
    std::for_each(unitsToErase.begin(), unitsToErase.end(), [this](Unit* each) { OnUnitExit(each); });
}

int UnitAcquisitionSphereCollider::GetTeamIndex()
{
    if (owner.expired())
        return teamIndex;
    else
        return owner.lock()->GetTeamIndex();
}
