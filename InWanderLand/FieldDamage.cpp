#include "FieldDamage.h"
#include "StatusTimer.h"
#include "StatusTimerPool.h"
#include "Unit.h"
#include "UnitTransformComponent.h"

void FieldDamage::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	UnitSensor* tempStruct = new UnitSensor;
	tempStruct->opponentUnit = opponentUnit;
	tempStruct->isInField = true;

	StatusTimer* inFieldTimer = StatusTimerPool::SingleInstance().Borrow();
	opponentUnitMap.insert({ tempStruct, inFieldTimer });
	
	inFieldTimer->m_isRepeated = true;
	inFieldTimer->m_duration = m_fieldDamageDelay;
	inFieldTimer->onCompleteFunction = [=]()
	{
		opponentUnit->Damaged(m_fieldDamage);
	};
	inFieldTimer->ActivateTimer();
}

void FieldDamage::Start()
{
}

void FieldDamage::OnTriggerExit(physics::Collider* collider)
{
	if (collider->GetGameObject()->GetComponent<UnitTransformComponent>() != nullptr &&
		collider->GetGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>() != nullptr &&
		collider->GetGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>()->GetUnitSide() == Unit::UnitSide::Enemy)
	{
		unitOnField = collider->GetGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>();

		for (auto e : opponentUnitMap)
		{
			if (e.first->opponentUnit == unitOnField)
			{
				e.second->StopTimer();
				opponentUnitMap.erase(e.first);
				break;
			}
		}
	}
}

