#include "InWanderLand.h"
#include "TauntingComponent.h"
#include "Unit.h"
#include "DebuggingMeshPool.h"
#include "StatusTimer.h"
#include "StatusTimerPool.h"

void TauntingComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
    float m_tauntingDuration = application::GlobalConstant::GetSingletonInstance().pod.robinESkillRadius;
    DamageOnlyComponent::ApplyStatus(ownerUnit, opponentUnit);

    /// 도발 기능 추가
    opponentUnit->ChangeCurrentOpponentUnitForced(ownerUnit);
    opponentUnit->ReportStatusEffectApplied(StatusEffect::StatusEffectEnum::Taunted);

    m_tauntTimer = StatusTimerPool::Instance().Borrow();

    m_tauntTimer->pushDuration = m_tauntingDuration;
    m_tauntTimer->onCompleteFunction = [=]()
        {
            opponentUnit->DeleteTauntingUnit();
            opponentUnit->ReportStatusEffectEnded(StatusEffect::StatusEffectEnum::Taunted);
        };
    m_tauntTimer->ActivateTimer();
}
