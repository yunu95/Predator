#include "TrapTriggerSensor.h"
#include "Unit.h"

void TrapTriggerSensor::OnTriggerEnter(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		trapChessSkillSystem->ActivateSkill(Unit::SkillEnum::BossSkillOne, Vector3d::zero);
	}
}
