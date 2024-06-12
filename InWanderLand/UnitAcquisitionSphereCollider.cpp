#include "UnitAcquisitionSphereCollider.h"
#include "InWanderLand.h"

bool UnitAcquisitionSphereCollider::ShouldContain(Unit* unit)
{
    return (includeDeadUnits || unit->IsAlive()) && (includeInvulnerableUnits || !unit->IsInvulenerable());
}

void UnitAcquisitionSphereCollider::OnTriggerEnter(physics::Collider* other)
{
    if (auto unit = other->GetGameObject()->GetComponent<Unit>(); unit)
    {
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

void UnitAcquisitionSphereCollider::OnTriggerExit(physics::Collider* other)
{
    if (!other) return;
    if (auto unit = other->GetGameObject()->GetComponent<Unit>(); unit)
    {
        unitsWhatSoEver.erase(unit);
        units.erase(unit);
        enemies.erase(unit);
        friends.erase(unit);
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
    units.clear();
    friends.clear();
    enemies.clear();
    std::copy_if(unitsWhatSoEver.begin(), unitsWhatSoEver.end(), std::inserter(units,units.end()), [this](Unit* const unit) { return ShouldContain(unit); });
    std::copy_if(units.begin(), units.end(), std::inserter(friends,friends.end()), [this](Unit* const unit) {return unit->GetTeamIndex() == GetTeamIndex(); });
    std::copy_if(units.begin(), units.end(), std::inserter(enemies,enemies.end()), [this](Unit* const unit) {return unit->GetTeamIndex() != GetTeamIndex(); });
}

int UnitAcquisitionSphereCollider::GetTeamIndex()
{
    if (owner.expired())
        return teamIndex;
    else
        return owner.lock()->GetTeamIndex();
}
