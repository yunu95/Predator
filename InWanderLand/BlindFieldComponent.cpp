#include "BlindFieldComponent.h"
#include "StatusTimerPool.h"
#include "Unit.h"

void BlindFieldComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	FieldDamage::ApplyStatus(ownerUnit, opponentUnit);
	
	/// 실명 부여 로직
	opponentUnit->SetUnitDamageToZero(true);
	opponentUnit->ReportStatusEffectApplied(StatusEffect::StatusEffectEnum::Blinding);

	m_blindTimer->m_isRepeated = false;
	m_blindTimer->pushDuration = m_blindPersistTime;
	m_blindTimer->onCompleteFunction = [=]()
	{
		/// 실명 해제 로직
		opponentUnit->ReportStatusEffectEnded(StatusEffect::StatusEffectEnum::Blinding);
		opponentUnit->SetUnitDamageToZero(false);
	};
}

void BlindFieldComponent::SetFieldSkillMembers()
{
	// 기본 장판 초기 설정
	m_damageTimer = StatusTimerPool::Instance().Borrow();
	m_fieldDamageDelay = 0.3f;
	m_fieldDamage = 1.0f;

	// 실명 효과 초기 설정
	m_blindTimer = StatusTimerPool::Instance().Borrow();
	m_blindPersistTime = 3.0f;
}
