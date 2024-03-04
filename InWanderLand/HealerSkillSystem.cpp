#include "HealerSkillSystem.h"

void HealerSkillSystem::SetSkillRequirmentsActive(SkillRequirements p_requirments, bool p_boolen)
{
	p_requirments.skillCollider->SetActive(p_boolen);
	p_requirments.colliderObject->SetSelfActive(p_boolen);
	p_requirments.debugObject->SetSelfActive(p_boolen);
}

void HealerSkillSystem::CrushDown(int p_times)
{
	static int crushedCount = 0;
	float yDistance = GetTransform()->GetWorldPosition().y;
	m_unitDotween->DOMove(GetTransform()->GetWorldPosition() - Vector3d(0, yDistance, 0), 0.3f).OnComplete([=]()
		{
			crushedCount++;

			SetSkillRequirmentsActive(QSkillFieldDamage, true);
			isQSkillColliderActivated = true;

			if (crushedCount == p_times)
			{
				crushedCount = 0;
				//GetTransform()->SetWorldPosition(startPosition);
				m_unitNavComponent->SetActive(true);
				m_unitNavComponent->AssignToNavigationField(m_unitComponent->GetNavField());
				m_unitNavComponent->Relocate(GetTransform()->GetWorldPosition());
				isSkillEnd = true;
			}

			else
			{
				m_unitDotween->DOMove(GetTransform()->GetWorldPosition() + Vector3d(0, yDistance, 0), 0.3f).OnComplete([=]()
					{
						SetSkillRequirmentsActive(QSkillFieldDamage, false);
						CrushDown(p_times);
					});
			}
		});
}

void HealerSkillSystem::SetQSkillCollider(physics::SphereCollider* p_fieldDamageCollider)
{
	QSkillFieldDamage.skillCollider = p_fieldDamageCollider;
}

void HealerSkillSystem::SetQSkillObject(GameObject* p_fieldDamageObj)
{
	QSkillFieldDamage.colliderObject = p_fieldDamageObj;
}

void HealerSkillSystem::SetQSkillDebugInfo(GameObject* p_fieldDebugObject)
{
	QSkillFieldDamage.debugObject = p_fieldDebugObject;
}

void HealerSkillSystem::SetWSkillCollider(physics::BoxCollider* p_fieldDamageCollider)
{
	WSkillFieldDamage.skillCollider = p_fieldDamageCollider;
}

void HealerSkillSystem::SetWSkillObject(GameObject* p_fieldDamageObj)
{
	WSkillFieldDamage.colliderObject = p_fieldDamageObj;
}

void HealerSkillSystem::SetWSkillDebugInfo(GameObject* p_fieldDebugObject)
{
	WSkillFieldDamage.debugObject = p_fieldDebugObject;
}

void HealerSkillSystem::QSkillActivate(Vector3d skillPos)
{
	startPosition = GetTransform()->GetWorldPosition();

	m_currentSelectedSkillPosition = skillPos;

	m_unitComponent->SetSkillDuration(4.0f);

	isQSkillActivating = true;

	m_unitNavComponent->SetActive(false);

	m_unitDotween->DOMove(skillPos + Vector3d({ 0, QSkillJumpYdistance , 0 }), reachTopTime).OnComplete([=]()
		{
			CrushDown(3);
		});
}

void HealerSkillSystem::WSkillActivate(Vector3d skillPos)
{
	m_unitComponent->SetSkillDuration(2.0f);

	isWSkillActivating = true;
	isWSkillColliderActivated = true;

	SetSkillRequirmentsActive(WSkillFieldDamage, true);


}

void HealerSkillSystem::Start()
{
	SetOtherComponentsAsMember();
	QSkillFieldDamage.skillCollider->SetActive(false);
}

void HealerSkillSystem::Update()
{
	if (isQSkillActivating)
	{
		QSkillFieldDamage.debugObject->GetTransform()->SetWorldPosition(QSkillFieldDamage.colliderObject->GetTransform()->GetWorldPosition());
		//m_unitComponent->LookAt(m_currentSelectedSkillPosition);
	}

	if (isWSkillActivating)
	{
		WSkillFieldDamage.colliderObject->GetTransform()
			->SetWorldPosition({GetGameObject()->GetTransform()->GetWorldPosition() + GetGameObject()->GetTransform()->GetWorldRotation().Forward() * -3});
		WSkillFieldDamage.colliderObject->GetTransform()->SetWorldRotation(GetGameObject()->GetTransform()->GetWorldRotation());

		WSkillFieldDamage.debugObject->GetTransform()
			->SetWorldPosition(WSkillFieldDamage.colliderObject->GetTransform()->GetWorldPosition());
		WSkillFieldDamage.debugObject->GetTransform()->SetWorldRotation(WSkillFieldDamage.colliderObject->GetTransform()->GetWorldRotation());

		//m_unitComponent->LookAt(m_currentSelectedSkillPosition);
	}

	if (!isColliderSetActiveFalse)
	{
		SetSkillRequirmentsActive(QSkillFieldDamage, false);
		SetSkillRequirmentsActive(WSkillFieldDamage, false);
		isColliderSetActiveFalse = true;
	}

	if (isQSkillColliderActivated)
	{
		QSkillColliderElapsed += Time::GetDeltaTime();
		if (QSkillColliderElapsed >= QSkillColliderRemainTime)
		{
			QSkillColliderElapsed = 0.0f;
			SetSkillRequirmentsActive(QSkillFieldDamage, false);
			isQSkillColliderActivated = false;
		}
	}

	if (isWSkillColliderActivated)
	{
		WSkillColliderElapsed += Time::GetDeltaTime();
		if (WSkillColliderElapsed >= WSkillColliderRemainTime)
		{
			WSkillColliderElapsed = 0.0f;
			SetSkillRequirmentsActive(WSkillFieldDamage, false);
			isWSkillColliderActivated = false;
		}
	}


}
