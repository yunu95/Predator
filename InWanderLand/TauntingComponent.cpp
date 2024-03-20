#include "InWanderLand.h"
#include "TauntingComponent.h"
#include "Unit.h"
#include "DebuggingMeshPool.h"
#include "StatusTimer.h"
#include "StatusTimerPool.h"

void TauntingComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	DamageOnlyComponent::ApplyStatus(ownerUnit, opponentUnit);

	/// 도발 기능 추가
	opponentUnit->ChangeCurrentOpponentUnitForced(ownerUnit);

	m_tauntTimer = StatusTimerPool::SingleInstance().Borrow();

	m_tauntTimer->m_duration = m_tauntingDuration;
	m_tauntTimer->onCompleteFunction = [=]()
	{
		opponentUnit->DeleteTauntingUnit();
	};
	m_tauntTimer->ActivateTimer();
}
