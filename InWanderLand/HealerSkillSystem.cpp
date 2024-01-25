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
	unitDotween->DOMove(GetTransform()->GetWorldPosition() - Vector3d(0, yDistance, 0), 0.3f).OnComplete([=]()
		{
			crushedCount++;

			SetSkillRequirmentsActive(QSkillFieldDamage, true);
			isQSkillColliderActivated = true;

			if (crushedCount == p_times)
			{
				crushedCount = 0;
				//GetTransform()->SetWorldPosition(startPosition);
				unitNavComponent->SetActive(true);
				unitNavComponent->Relocate(GetTransform()->GetWorldPosition());
			}

			else
			{
				unitDotween->DOMove(GetTransform()->GetWorldPosition() + Vector3d(0, yDistance, 0), 0.3f).OnComplete([=]()
					{
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

void HealerSkillSystem::SetQSkillDebugInfo(GameObject* p_fieldDebugObject, float p_fieldDamageRadius)
{
	QSkillFieldDamage.debugObject = p_fieldDebugObject;
	QSkillFieldRadius = p_fieldDamageRadius;
}

void HealerSkillSystem::QSkillActivate(Vector3d skillPos)
{
	startPosition = GetTransform()->GetWorldPosition();

	isQSkillActivating = true;

	unitNavComponent->SetActive(false);

	unitDotween->DOMove(skillPos + Vector3d(0, QSkillJumpYdistance, 0), reachTopTime).OnComplete([=]()
		{
			CrushDown(3);
		});
}

void HealerSkillSystem::WSkillActivate(Vector3d skillPos)
{

}

void HealerSkillSystem::Start()
{
	unitDotween = GetGameObject()->GetComponent<Dotween>();
	unitNavComponent = GetGameObject()->GetComponent<NavigationAgent>();
	QSkillFieldDamage.skillCollider->SetActive(false);
}

void HealerSkillSystem::Update()
{
	if (isQSkillActivating)
	{
		QSkillFieldDamage.debugObject->GetTransform()->SetWorldPosition(QSkillFieldDamage.colliderObject->GetTransform()->GetWorldPosition());
	}

	if (!isColliderSetActiveFalse)
	{
		SetSkillRequirmentsActive(QSkillFieldDamage, false);
		isColliderSetActiveFalse = true;
	}

	if (isQSkillColliderActivated)
	{
		QSkillColliderElapsed += Time::GetDeltaTime();
		if (QSkillColliderElapsed >= QSkillColliderRemainTime)
		{
			SetSkillRequirmentsActive(QSkillFieldDamage, false);
			isQSkillColliderActivated = false;
		}
	}


}
