#include "ParalysisFieldComponent.h"
#include "StatusTimerPool.h"
#include "Unit.h"
#include "InWanderLand.h"

void ParalysisFieldComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
    FieldDamage::ApplyStatus(ownerUnit, opponentUnit);

    m_onFieldUnitsMap.insert({ opponentUnit, true });

    opponentUnit->ReportStatusEffectApplied(StatusEffect::StatusEffectEnum::Paralysis);
    opponentUnit->MakeUnitParalysisState();
    opponentUnit->KnockBackUnit(GetTransform()->GetWorldPosition(), 0.5f);
    opponentUnit->paralysisTimer->pushDuration = application::GlobalConstant::GetSingletonInstance().pod.ursulaESkillStunDuration;
    opponentUnit->paralysisTimer->pushDuration = application::GlobalConstant::GetSingletonInstance().pod.ursulaESkillPullingDuration;
    opponentUnit->paralysisTimer->ActivateTimer();
    /*opponentUnit->GetGameObject()->GetComponent<Dotween>()->DOMove(GetTransform()->GetWorldPosition(), m_pullingTime).SetEase(EaseOutCubic).OnComplete([=]()
        {
            tempNavComponent->SetActive(true);
            tempNavComponent->AssignToNavigationField(opponentUnit->GetNavField());
            tempNavComponent->Relocate(GetTransform()->GetWorldPosition());

            opponentUnit->MultipleUnitSpeed(1 / m_slowMultipleScale);
            opponentUnit->ReportStatusEffectEnded(StatusEffect::StatusEffectEnum::Paralysis);
            opponentUnit->SetUnitStateIdle();
        });	*/
}

void ParalysisFieldComponent::SetFieldSkillMembers()
{
    // 기본 장판 초기 설정
    m_damageTimer = StatusTimerPool::Instance().Borrow();
    m_fieldDamageDelay = application::GlobalConstant::GetSingletonInstance().pod.ursulaESkillDamageInterval;
    m_fieldDamage = application::GlobalConstant::GetSingletonInstance().pod.ursulaESkillDamagePerTick;

    // 경직 효과 초기 설정
    //m_paralysisTimer = StatusTimerPool::Instance().Borrow();
    //m_paralysisTime = 3.0f;

    // 슬로우 배율 설정
    m_slowMultipleScale = 1.5f;

    // 끌어당기는 힘 설정
    //m_pullingPower = 3.0f;
    //m_pullingTime = 0.5f;
}

void ParalysisFieldComponent::OnTriggerExit(physics::Collider* collider)
{
    if (Unit* colliderUnitComponent = UnitCollider::AcquireUnit(collider);
        colliderUnitComponent != nullptr &&
        colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Enemy)
    {
        colliderUnitComponent->MultipleUnitSpeed(m_slowMultipleScale);
        m_currentOnFieldUnits.erase(colliderUnitComponent);
    }
}

