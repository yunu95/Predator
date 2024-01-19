#include "FieldDamage.h"
#include "StatusTimer.h"
#include "StatusTimerPool.h"
#include "Unit.h"

void FieldDamage::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	UnitSensor* tempStruct = new UnitSensor;
	tempStruct->opponentUnit = opponentUnit;
	tempStruct->isInField = true;

	m_timer = StatusTimerPool::SingleInstance().Borrow();
	opponentUnitMap.insert({ tempStruct, m_timer });
	
	m_timer->m_isRepeated = true;
	m_timer->m_duration = m_fieldDamageDelay;
	//inFieldTimer->onCompleteFunction = [=]()
	//{
	//	opponentUnit->Damaged(m_fieldDamage);
	//};
	//inFieldTimer->ActivateTimer();
}

void FieldDamage::Start()
{
	m_timer = StatusTimerPool::SingleInstance().Borrow();
}

void FieldDamage::OnDestroy()
{
	for (auto e : opponentUnitMap)
	{
		if (e.first->opponentUnit != nullptr)
			delete e.first;
	}
}

void FieldDamage::OnTriggerExit(physics::Collider* collider)
{
	if (Unit * colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Enemy)
	{
		for (auto e : opponentUnitMap)
		{
			if (e.first->opponentUnit == colliderUnitComponent)
			{
				e.second->StopTimer();
				opponentUnitMap.erase(e.first);
				break;
			}
		}
	}
}

void FieldDamage::ActivateFieldTimer()
{
	m_timer->ActivateTimer();
}

