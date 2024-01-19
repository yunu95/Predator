#include "BlindFieldComponent.h"
#include "StatusTimerPool.h"
#include "Unit.h"

void BlindFieldComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	m_fieldDamageDelay = 0.3f;
	m_fieldDamage = 1.0f;

	FieldDamage::ApplyStatus(ownerUnit, opponentUnit);

	m_timer->onCompleteFunction = [=]()
	{
		opponentUnit->Damaged(m_fieldDamage);
		/// 실명 로직

	};
	m_timer->ActivateTimer();
}
