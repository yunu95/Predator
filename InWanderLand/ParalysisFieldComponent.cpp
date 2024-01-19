#include "ParalysisFieldComponent.h"
#include "StatusTimerPool.h"
#include "Unit.h"

void ParalysisFieldComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	m_fieldDamageDelay = 0.2f;
	m_fieldDamage = 0.5f;

	FieldDamage::ApplyStatus(ownerUnit, opponentUnit);

	auto tempNavComponent = opponentUnit->GetGameObject()->GetComponent<NavigationAgent>();

	m_timer->onCompleteFunction = [=]()
	{
		opponentUnit->Damaged(m_fieldDamage);
		tempNavComponent->SetActive(false);

	};
	m_timer->ActivateTimer();
	
}

