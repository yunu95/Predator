#include "InWanderLand.h"
#include "WarriorSkillSystem.h"
#include "KnockBackComponent.h"
#include "Dotween.h"
#include "ContentsLayer.h"
#include "Application.h"

void WarriorSkillSystem::ActivateSkillOne(Vector3d skillPos)
{
	QknockBackSkill.colliderObject->GetComponent<KnockBackComponent>()->SkillStarted();
	QknockBackSkill.colliderObject->SetParent(GetGameObject());

	SetSkillRequirmentsActive(QknockBackSkill, true);

	m_unitNavComponent->SetActive(false);

	float tempDistance = (skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();
	m_unitComponent->SetSkillDuration(tempDistance / m_QskillRushSpeed);
	// 1. 목표 위치로 돌진
	m_unitDotween->DOMove(skillPos, tempDistance / m_QskillRushSpeed).OnComplete([=]()
		{
			SetSkillRequirmentsActive(QknockBackSkill, false);

			QknockBackSkill.colliderObject->GetComponent<KnockBackComponent>()->ClearCrushedUnitList();

			m_unitNavComponent->SetActive(true);
			m_unitNavComponent->AssignToNavigationField(m_unitComponent->GetNavField());
			m_unitNavComponent->Relocate(skillPos);
		});
}

void WarriorSkillSystem::ActivateSkillTwo(Vector3d skillPos)
{
	SetSkillRequirmentsActive(WTauntSkill, true);

	m_unitComponent->SetSkillDuration(m_wSkillColliderRemainTime);

	m_unitDotween->DONothing(m_wSkillColliderRemainTime).OnComplete([=]()
		{
			SetSkillRequirmentsActive(WTauntSkill, false);
		});
	m_developedFunctionToWSkill();
}

void WarriorSkillSystem::SetQSkillKnockBackObject(GameObject* obj)
{
	QknockBackSkill.colliderObject = obj;
}

void WarriorSkillSystem::SetWSkillObject(GameObject* obj)
{
	WTauntSkill.colliderObject = obj;
}

void WarriorSkillSystem::SetKnockBackDebugObject(GameObject* obj, float radius)
{
	QknockBackSkill.debugObject = obj;
	m_QSkillRadius = radius;
}

void WarriorSkillSystem::SetWSkillDebugObject(GameObject* obj, float radius)
{
	WTauntSkill.debugObject = obj;
	m_WSkillRadius = radius;
}

void WarriorSkillSystem::Start()
{
	SetOtherComponentsAsMember();

	SetSkillRequirmentsActive(QknockBackSkill, false);
	SetSkillRequirmentsActive(WTauntSkill, false);

	QknockBackSkill.colliderObject->SetParent(GetGameObject());
	QknockBackSkill.debugObject->SetParent(GetGameObject());
	WTauntSkill.colliderObject->SetParent(GetGameObject());
	WTauntSkill.debugObject->SetParent(GetGameObject());

	m_developedFunctionToWSkill = []() {};
}

void WarriorSkillSystem::Update()
{
}
