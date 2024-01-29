#include "InWanderLand.h"
#include "WarriorSkillSystem.h"
#include "KnockBackComponent.h"
#include "Dotween.h"

void WarriorSkillSystem::QSkillActivate(Vector3d skillPos)
{
	QSkillKnockBackCollider->GetGameObject()->GetComponent<KnockBackComponent>()->SkillStarted();

	QSkillKnockBackCollider->SetActive(true);

	m_unitNavComponent->SetActive(false);	

	float tempDistance = (skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

	isQSkillActivating = true;
	m_QSkillKnockBackDebugObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
	m_QSkillKnockBackDebugObject->SetSelfActive(true);

	m_unitComponent->SetSkillDuration(tempDistance / m_QskillRushSpeed);

	// 1. 목표 위치로 돌진
	m_unitDotween->DOMove(skillPos, tempDistance / m_QskillRushSpeed).OnComplete([=]()
		{
			// 2. remainTime 이후에 collider 비활성화
			m_unitDotween->DONothing(m_knockBackObjectRemainTime).OnComplete([=]()
				{
					isQSkillActivating = false;
					m_QSkillKnockBackDebugObject->SetSelfActive(false);
					QSkillKnockBackCollider->SetActive(false);
					QSkillKnockBackCollider->GetGameObject()->GetComponent<KnockBackComponent>()->SkillEnded();
					m_unitComponent->EndSkillState();			/// skill 종료 시 Unit을 idle상태로 전환.

					m_unitNavComponent->SetActive(true);
					m_unitNavComponent->AssignToNavigationField(m_unitComponent->GetNavField());
					m_unitNavComponent->Relocate(skillPos);
				});
		});
}

void WarriorSkillSystem::WSkillActivate(Vector3d skillPos)
{
	WSkillCollider->SetActive(true);
	m_WSkillDebugObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
	m_WSkillDebugObject->SetSelfActive(true);

	m_unitDotween->DONothing(m_wSkillColliderRemainTime).OnComplete([=]()
		{
			WSkillCollider->SetActive(false);
			m_WSkillDebugObject->SetSelfActive(false);
		});
}

void WarriorSkillSystem::SetQSkillKnockBackCollider(physics::SphereCollider* p_collider)
{
	QSkillKnockBackCollider = p_collider;
}

void WarriorSkillSystem::SetWSkillCollider(physics::SphereCollider* p_collider)
{
	WSkillCollider = p_collider;
}

void WarriorSkillSystem::SetKnockBackDebugObject(GameObject* obj, float radius)
{
	m_QSkillKnockBackDebugObject = obj;
	m_QSkillRadius = radius;
}

void WarriorSkillSystem::SetWSkillDebugObject(GameObject* obj, float radius)
{
	m_WSkillDebugObject = obj;
	m_WSkillRadius = radius;
}

void WarriorSkillSystem::Start()
{
	SetOtherComponentsAsMember();

	m_QSkillKnockBackDebugObject->GetTransform()->scale = { pow(m_QSkillRadius, 2), pow(m_QSkillRadius, 2), pow(m_QSkillRadius, 2) };
	m_QSkillKnockBackDebugObject->SetSelfActive(false);

	m_WSkillDebugObject->GetTransform()->scale = { pow(m_WSkillRadius, 2), pow(m_WSkillRadius, 2), pow(m_WSkillRadius, 2) };
	m_WSkillDebugObject->SetSelfActive(false);
}

void WarriorSkillSystem::Update()
{
	/// Update 들어가기 전에는 Collider->SetActive(false) 불가하므로 첫 Update문에서 false 처리. (임시)
	if (!isColliderSetActiveFalseSet)
	{
		QSkillKnockBackCollider->SetActive(false);
		WSkillCollider->SetActive(false);
		isColliderSetActiveFalseSet = true;
	}

	if (isQSkillActivating)
	{
		m_QSkillKnockBackDebugObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
	}
	if (isWSkillActivating)
	{
		m_WSkillDebugObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
	}
}
