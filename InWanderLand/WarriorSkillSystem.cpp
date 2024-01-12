#include "InWanderLand.h"
#include "WarriorSkillSystem.h"
#include "Dotween.h"

void WarriorSkillSystem::QSkillActivate(Vector3d skillPos)
{
	QSkillKnockBackCollider->SetActive(true);

	GetGameObject()->GetComponent<NavigationAgent>()->SetActive(false);	

	float tempDistance = (skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

	// 1. 목표 위치로 돌진
	m_UnitDotween->DOMove(skillPos, tempDistance / m_QskillRushSpeed).OnComplete([=]()
		{
			// 2. remainTime 이후에 collider 비활성화
			m_UnitDotween->DONothing(m_knockBackObjectRemainTime).OnComplete([=]()
				{
					QSkillKnockBackCollider->SetActive(false);
					GetGameObject()->GetComponent<Unit>()->EndSkillState();			/// skill 종료 시 Unit을 idle상태로 전환.

					GetGameObject()->GetComponent<NavigationAgent>()->SetActive(true);
					GetGameObject()->GetComponent<NavigationAgent>()->AssignToNavigationField(GetGameObject()->GetComponent<Unit>()->GetNavField());
					GetGameObject()->GetComponent<NavigationAgent>()->Relocate(skillPos);
				});
		});
}

void WarriorSkillSystem::WSkillActivate(Vector3d skillPos)
{
	WSkillCollider->SetActive(true);

	m_UnitDotween->DONothing(m_wSkillColliderRemainTime).OnComplete([=]()
		{
			WSkillCollider->SetActive(false);
		});
}

void WarriorSkillSystem::ESkillActivate(Vector3d skillPos)
{

}

void WarriorSkillSystem::SetQSkillKnockBackCollider(physics::SphereCollider* p_collider)
{
	QSkillKnockBackCollider = p_collider;
}

void WarriorSkillSystem::SetWSkillCollider(physics::SphereCollider* p_collider)
{
	WSkillCollider = p_collider;
}

void WarriorSkillSystem::SetUnitTransformObject(GameObject* obj)
{
	m_unitTransformObject = obj;
}

void WarriorSkillSystem::Start()
{
	SetPlayerDotweenComponent(GetGameObject()->GetComponent<Dotween>());
	m_totalSkillTime = 5.0f;
	//QSkillKnockBackObject->SetSelfActive(false);
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
}
