#include "BleedingComponent.h"
#include "StatusTimerPool.h"
#include "StatusTimer.h"
#include "UnitTransformComponent.h"
#include "Unit.h"

void BleedingComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	/// 출혈 시스템	
	/// damage를 주는건 Unit->Damaged 함수를 호출,
	/// 이외의 로직(출혈 스택, 지속 피해, )은 내부에서 구현
	
	StatusTimer* bleedingTimer = StatusTimerPool::GetInstance()->GetStatusTimer();
	opponentUnitMap.insert({ opponentUnit, bleedingTimer });

	bleedingTimer->m_isRepeated = true;
	bleedingTimer->m_duration = m_bleedDuration;
	bleedingTimer->onCompleteFunction = [=]()
	{
		if (m_currentDamagedCount == m_maxDamagedCount)
		{
			m_currentDamagedCount = 0;
			bleedingTimer->StopTimer();
			opponentUnitMap.erase(opponentUnit);
			StatusTimerPool::GetInstance()->ReturnToPool(bleedingTimer);
		}

		else
		{
			opponentUnit->Damaged(m_bleedDamage);
			m_currentDamagedCount++;
		}
	};

	bleedingTimer->ActivateTimer();
}

void BleedingComponent::Update()
{
	
}

void BleedingComponent::OnTriggerEnter(physics::Collider* collider)
{
	// Request StatusTimer To TimerPool here
	if (collider->GetGameObject()->GetComponent<UnitTransformComponent>() != nullptr &&
		collider->GetGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>() != nullptr &&
		collider->GetGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>()->GetUnitSide() == Unit::UnitSide::Enemy)
	{
		ApplyStatus(GetGameObject()->GetParentGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>(),
			collider->GetGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>());
	}
}
