#include "HealerSkillSystem.h"
#include "ContentsLayer.h"
#include "Application.h"

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

void HealerSkillSystem::SetQSkillObject(GameObject* p_fieldDamageObj)
{
	QSkillFieldDamage.colliderObject = p_fieldDamageObj;
}

void HealerSkillSystem::SetQSkillDebugInfo(GameObject* p_fieldDebugObject)
{
	QSkillFieldDamage.debugObject = p_fieldDebugObject;
}

void HealerSkillSystem::SetWSkillObject(GameObject* p_fieldDamageObj)
{
	WSkillFieldDamage.colliderObject = p_fieldDamageObj;
}

void HealerSkillSystem::SetWSkillDebugInfo(GameObject* p_fieldDebugObject)
{
	WSkillFieldDamage.debugObject = p_fieldDebugObject;
}

void HealerSkillSystem::ActivateSkillOne(Vector3d skillPos)
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

void HealerSkillSystem::ActivateSkillTwo(Vector3d skillPos)
{
	m_unitComponent->SetSkillDuration(2.0f);

	isWSkillActivating = true;
	isWSkillColliderActivated = true;

	SetSkillRequirmentsActive(WSkillFieldDamage, true);


}

void HealerSkillSystem::Start()
{
	SetOtherComponentsAsMember();

	QSkillFieldDamage.colliderObject->SetParent(GetGameObject());
	QSkillFieldDamage.debugObject->SetParent(GetGameObject());
	WSkillFieldDamage.colliderObject->SetParent(GetGameObject());
	WSkillFieldDamage.debugObject->SetParent(GetGameObject());
}

void HealerSkillSystem::Update()
{
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
