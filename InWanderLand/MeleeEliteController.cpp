#include "MeleeEliteController.h"
#include "InWanderLand.h"

coroutine::Coroutine MeleeEliteController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
	co_yield coroutine::WaitForSeconds{ EnemySpinAttackSkill::pod.skillCoolTime };
	unit.lock()->OrderSkill(EnemySpinAttackSkill{}, unit.lock()->GetTransform()->GetWorldPosition());
}
