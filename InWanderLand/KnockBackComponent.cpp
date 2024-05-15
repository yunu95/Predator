#include "InWanderLand.h"
#include "KnockBackComponent.h"
#include "Unit.h"
#include "Dotween.h"

void KnockBackComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
    /// 넉백 시스템
    /// opponent 유닛을 밀어 내야 하는데...
    /// navigationAgent를 끄고 DOMove 후 OnComplete 에 navigationAgent를 붙여준다?

    bool isAlreadyCrushed = false;

    for (auto each : crushedUnitList)
    {
        if (each == opponentUnit)
        {
            isAlreadyCrushed = true;
            break;
        }
    }

    if (!isAlreadyCrushed)
    {
        crushedUnitList.push_back(opponentUnit);
        opponentUnit->ReportStatusEffectApplied(StatusEffect::StatusEffectEnum::KnockBack);
        opponentUnit->MakeUnitPushedState(true);
        opponentUnit->Damaged(ownerUnit, ownerUnit->DetermineAttackDamage(m_damage));
        Vector3d startPosition = opponentUnit->GetGameObject()->GetTransform()->GetWorldPosition();

        Vector3d directionVector = (startPosition - GetGameObject()->GetTransform()->GetWorldPosition()).Normalized();
        Vector3d endPosition = startPosition + (directionVector * pushDistance);
        Vector3d maxHeightPosition = startPosition + (directionVector * pushDistance / 2) + Vector3d(0, maxKnockHeight, 0);
        endPosition = opponentUnit->GetNavField()->GetClosestPointOnField(endPosition);

        opponentUnit->KnockBackUnit(endPosition, 0.5f);
    }
}

void KnockBackComponent::Update()
{

}

void KnockBackComponent::SetAP(float p_ap)
{
    m_damage = p_ap;
}

void KnockBackComponent::SkillStarted()
{
    isSkillStarted = true;
}

void KnockBackComponent::ClearCrushedUnitList()
{
    isSkillStarted = false;
    crushedUnitList.clear();
}

