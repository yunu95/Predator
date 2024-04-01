#include "InWanderLand.h"
#include "WarriorSkillSystem.h"
#include "KnockBackComponent.h"
#include "Dotween.h"

void WarriorSkillSystem::SetSkillRequirmentsActive(SkillRequirements p_requirments, bool p_boolen)
{
	p_requirments.skillCollider->SetActive(p_boolen);
	//p_requirments.colliderObject->SetSelfActive(p_boolen);
	p_requirments.debugObject->SetSelfActive(p_boolen);
}

void WarriorSkillSystem::QSkillActivate(Vector3d skillPos)
{
	isQSkillStarted = true;

	QknockBackSkill.colliderObject->GetComponent<KnockBackComponent>()->SkillStarted();

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

			isQSkillStarted = false;
		});
}

void WarriorSkillSystem::WSkillActivate(Vector3d skillPos)
{
	isWSkillStarted = true;

	SetSkillRequirmentsActive(WTauntSkill, true);

	m_unitComponent->SetSkillDuration(m_wSkillColliderRemainTime);

	m_unitDotween->DONothing(m_wSkillColliderRemainTime).OnComplete([=]()
		{
			SetSkillRequirmentsActive(WTauntSkill, false);
			isWSkillStarted = false;
		});
	m_developedFunctionToWSkill();
}

void WarriorSkillSystem::SetQSkillKnockBackObject(GameObject* obj)
{
	QknockBackSkill.colliderObject = obj;
	QknockBackSkill.skillCollider = obj->GetComponent<physics::Collider>();
}

void WarriorSkillSystem::SetWSkillObject(GameObject* obj)
{
	WTauntSkill.colliderObject = obj;
	WTauntSkill.skillCollider = obj->GetComponent<physics::Collider>();
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

	m_developedFunctionToWSkill = []() {};
}

void WarriorSkillSystem::Update()
{

	//QknockBackSkill.colliderObject->GetTransform()->SetLocalPosition(Vector3d::zero);
	if (isQSkillStarted)
	{
		QknockBackSkill.debugObject->GetTransform()->SetWorldPosition(QknockBackSkill.colliderObject->GetTransform()->GetWorldPosition());
	}

	if (isWSkillStarted)
	{
		WTauntSkill.debugObject->GetTransform()->SetWorldPosition(WTauntSkill.colliderObject->GetTransform()->GetWorldPosition());
	}
}
