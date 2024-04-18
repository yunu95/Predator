#include "TrapTriggerSensor.h"
#include "Unit.h"

void TrapTriggerSensor::OnTriggerEnter(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		trapUnit->SetUnitStateToSkill();
	}
}
 