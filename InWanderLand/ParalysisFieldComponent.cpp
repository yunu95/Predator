#include "ParalysisFieldComponent.h"
#include "StatusTimerPool.h"
#include "Unit.h"

void ParalysisFieldComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	FieldDamage::ApplyStatus(ownerUnit, opponentUnit);

	m_onFieldUnitsMap.insert({ opponentUnit, true });

	opponentUnit->MakeUnitParalysisState();

	auto tempNavComponent = opponentUnit->GetGameObject()->GetComponent<NavigationAgent>();
	tempNavComponent->SetActive(false);

	opponentUnit->GetGameObject()->GetComponent<Dotween>()->DOMove(GetTransform()->GetWorldPosition(), m_pullingTime).SetEase(EaseOutCubic).OnComplete([=]()
		{
			tempNavComponent->SetActive(true);
			tempNavComponent->AssignToNavigationField(opponentUnit->GetNavField());
			tempNavComponent->Relocate(GetTransform()->GetWorldPosition());

			opponentUnit->MultipleUnitSpeed(1 / m_slowMultipleScale);
			opponentUnit->MakeUnitStateIdle();
		});	
}

void ParalysisFieldComponent::SetFieldSkillMembers()
{
	// 기본 장판 초기 설정
	m_damageTimer = StatusTimerPool::SingleInstance().Borrow();
	m_fieldDamageDelay = 0.3f;
	m_fieldDamage = 1.0f;

	// 경직 효과 초기 설정
	m_paralysisTimer = StatusTimerPool::SingleInstance().Borrow();
	m_paralysisTime = 3.0f;
	
	// 슬로우 배율 설정
	m_slowMultipleScale = 1.5f;

	// 끌어당기는 힘 설정
	m_pullingPower = 3.0f;
	m_pullingTime = 0.5f;
}

void ParalysisFieldComponent::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Enemy)
	{
		colliderUnitComponent->MultipleUnitSpeed(m_slowMultipleScale);
		m_currentOnFieldUnits.erase(colliderUnitComponent);
	}
}

