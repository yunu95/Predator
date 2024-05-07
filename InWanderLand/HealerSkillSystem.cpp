#include "HealerSkillSystem.h"
#include "TacticModeSystem.h"

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
				isOncedActivated = false;
				crushedCount = 0;
				m_unitNavComponent->SetActive(true);
				m_unitNavComponent->AssignToNavigationField(m_unitComponent->GetNavField());
				m_unitNavComponent->Relocate(GetTransform()->GetWorldPosition());
				TacticModeSystem::Instance().ReportTacticActionFinished();
				SetSkillRequirmentsActive(QSkillFieldDamage, false);
				m_unitComponent->isPermittedToTacticAction = false;
				m_unitComponent->SetUnitStateIdle();
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
	isQSkillReady = false;

	startPosition = GetTransform()->GetWorldPosition();

	skillPos = CheckSkillRange(skillPos, Unit::SkillEnum::Q);

	m_currentSelectedSkillPosition = skillPos;

	isQSkillActivating = true;

	m_unitNavComponent->SetActive(false);

	m_unitDotween->DOMove(skillPos + Vector3d({ 0, QSkillJumpYdistance , 0 }), reachTopTime).OnComplete([=]()
		{
			CrushDown(3);
		});
}

void HealerSkillSystem::ActivateSkillTwo(Vector3d skillPos)
{
	isESkillReady = false;

	float skillWidth = 2.0f * UNIT_LENGTH;
	float skillHeight = m_skillTwoRange;

	WSkillFieldDamage.colliderObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
	WSkillFieldDamage.debugObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());

	WSkillFieldDamage.colliderObject->GetTransform()->SetWorldRotation(Vector3d::zero);
	WSkillFieldDamage.debugObject->GetTransform()->SetWorldRotation(Vector3d::zero);

	RotateProjectile(WSkillFieldDamage.colliderObject, skillPos);
	RotateProjectile(WSkillFieldDamage.debugObject, skillPos);

	m_wSkillColliderComponent->SetHalfExtent({ skillWidth / 2 , skillWidth / 2 , skillHeight / 2 });
	WSkillFieldDamage.debugObject->GetTransform()->SetWorldScale({ skillWidth, skillWidth, skillHeight });
	
	Vector3d tempSkillPosition = m_unitComponent->GetTransform()->GetWorldPosition() +
		(skillPos - m_unitComponent->GetTransform()->GetWorldPosition()).Normalized() *
		WSkillFieldDamage.debugObject->GetTransform()->GetWorldScale().z / 2;

	WSkillFieldDamage.colliderObject->GetTransform()->SetWorldPosition(tempSkillPosition);
	WSkillFieldDamage.debugObject->GetTransform()->SetWorldPosition(tempSkillPosition);

	Vector3d distanceVector = skillPos - GetTransform()->GetWorldPosition();
	float distance = distanceVector.Magnitude();

	isWSkillActivating = true;
	isWSkillColliderActivated = true;

	SetSkillRequirmentsActive(WSkillFieldDamage, true);
}

void HealerSkillSystem::SetSkillRequirmentLocalTimeScale(float p_scale)
{
	if (QSkillFieldDamage.dotweenComponent)
		LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(QSkillFieldDamage.dotweenComponent, p_scale);
	if (WSkillFieldDamage.dotweenComponent)
		LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(WSkillFieldDamage.dotweenComponent, p_scale);
}

void HealerSkillSystem::Start()
{
	SetOtherComponentsAsMember();

	m_wSkillColliderComponent = WSkillFieldDamage.colliderObject->GetComponent<physics::BoxCollider>();

	QSkillFieldDamage.colliderObject->SetParent(GetGameObject());
	QSkillFieldDamage.debugObject->SetParent(GetGameObject());

	SetSkillRequirmentsActive(QSkillFieldDamage, false);
	SetSkillRequirmentsActive(WSkillFieldDamage, false);

	//WSkillFieldDamage.colliderObject->SetParent(GetGameObject());
	//WSkillFieldDamage.debugObject->SetParent(GetGameObject());
}

void HealerSkillSystem::Update()
{
	PlayerSkillSystem::Update();

	//if (!isColliderSetActiveFalse)
	//{
	//	SetSkillRequirmentsActive(QSkillFieldDamage, false);
	//	SetSkillRequirmentsActive(WSkillFieldDamage, false);
	//	isColliderSetActiveFalse = true;
	//}

	//if (isQSkillColliderActivated)
	//{
	//	QSkillColliderElapsed += Time::GetDeltaTime() * m_localTimeScale;
	//	if (QSkillColliderElapsed >= QSkillColliderRemainTime)
	//	{
	//		QSkillColliderElapsed = 0.0f;
	//		SetSkillRequirmentsActive(QSkillFieldDamage, false);
	//		isQSkillColliderActivated = false;
	//		m_unitComponent->SetUnitStateIdle();
	//	}
	//}

	if (isWSkillColliderActivated)
	{
		WSkillColliderElapsed += Time::GetDeltaTime() * m_localTimeScale;
		if (WSkillColliderElapsed >= WSkillColliderRemainTime)
		{
			isOncedActivated = false;
			WSkillColliderElapsed = 0.0f;
			SetSkillRequirmentsActive(WSkillFieldDamage, false);
			isWSkillColliderActivated = false;
			TacticModeSystem::Instance().ReportTacticActionFinished();
			m_unitComponent->isPermittedToTacticAction = false;
			m_unitComponent->SetUnitStateIdle();
		}
	}
}
