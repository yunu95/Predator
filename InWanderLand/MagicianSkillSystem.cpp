#include "MagicianSkillSystem.h"
#include "Dotween.h"
#include "TacticModeSystem.h"

void MagicianSkillSystem::ActivateSkillOne(Vector3d skillPos)
{
	/// 장판의 타이머를 Activate하는 함수가 필요...
	//m_QSkillComponent->ActivateFieldTimer();

	isQSkillActivating = true;

	QSkillProjectile.colliderObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());	
	QSkillProjectile.colliderObject->GetTransform()->SetWorldRotation(Quaternion(Vector3d::zero));
	QSkillProjectile.debugObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());	

	SetSkillRequirmentsActive(QSkillProjectile, true);

	skillPos = CheckSkillRange(skillPos, Unit::SkillEnum::Q);

	RotateProjectile(QSkillProjectile.colliderObject, skillPos);

	QSkillFieldDamage.colliderObject->GetTransform()->SetWorldPosition(skillPos);			// 오브젝트만 움직여도 collider와 debug는 따라올 것이다.

	float tempDistance = (skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

	QSkillProjectile.dotweenComponent->DOMove(skillPos, tempDistance / m_QSkillProjectileSpeed).OnComplete([=]()
		{
			SetSkillRequirmentsActive(QSkillProjectile, false);
			SetSkillRequirmentsActive(QSkillFieldDamage, true);

			TacticModeSystem::Instance().ReportTacticActionFinished();
			m_unitComponent->isPermittedToTacticAction = false;
			m_unitComponent->SetUnitStateIdle();

			QSkillFieldDamage.dotweenComponent->DONothing(m_QSkillFieldRemainTime).OnComplete([=]()
				{
					SetSkillRequirmentsActive(QSkillFieldDamage, false);
					isQSkillActivating = false;
				});
		});
}

void MagicianSkillSystem::ActivateSkillTwo(Vector3d skillPos)
{
	isWSkillActivating = true;

	WSkillProjectile.colliderObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
	WSkillProjectile.debugObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());

	SetSkillRequirmentsActive(WSkillProjectile, true);

	skillPos = CheckSkillRange(skillPos, Unit::SkillEnum::W);
	WSkillFieldDamage.colliderObject->GetTransform()->SetWorldPosition(skillPos);			// 오브젝트만 움직여도 collider와 debug는 따라올 것이다.

	float tempDistance = (skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();
	WSkillProjectile.dotweenComponent->DOMove(skillPos, tempDistance / m_WSkillProjectileSpeed).OnComplete([=]()
		{
			SetSkillRequirmentsActive(WSkillProjectile, false);
			SetSkillRequirmentsActive(WSkillFieldDamage, true);

			TacticModeSystem::Instance().ReportTacticActionFinished();
			m_unitComponent->isPermittedToTacticAction = false;
			m_unitComponent->SetUnitStateIdle();

			WSkillFieldDamage.dotweenComponent->DONothing(m_WSkillFieldRemainTime).OnComplete([=]()
				{
					SetSkillRequirmentsActive(WSkillFieldDamage, false);
					isWSkillActivating = false;
				});
		});
}

void MagicianSkillSystem::SetInterActionComponent(BlindFieldComponent* p_QSkillComponent, ParalysisFieldComponent* p_WSkillComponent)
{
	m_QSkillComponent = p_QSkillComponent;
	m_WSkillComponent = p_WSkillComponent;
}

void MagicianSkillSystem::SetQSkillObject(GameObject* p_projectileObj, GameObject* p_fieldDamageObj)
{
	QSkillProjectile.colliderObject = p_projectileObj;
	QSkillFieldDamage.colliderObject = p_fieldDamageObj;

	QSkillProjectile.dotweenComponent = p_projectileObj->GetComponent<Dotween>();
	QSkillFieldDamage.dotweenComponent = p_fieldDamageObj->GetComponent<Dotween>();
}

void MagicianSkillSystem::SetQSkillDebugPair(std::pair<GameObject*, float> p_projectileObjectPair, std::pair<GameObject*, float> p_fieldObjectPair)
{
	QSkillProjectile.debugObject = p_projectileObjectPair.first;
	QSkillFieldDamage.debugObject = p_fieldObjectPair.first;

	m_QSkillProjectileRadius = p_projectileObjectPair.second;
	m_QSkillFieldRadius = p_fieldObjectPair.second;
}


void MagicianSkillSystem::SetWSkillObject(GameObject* p_projectileObj, GameObject* p_fieldDamageObj)
{
	WSkillProjectile.colliderObject = p_projectileObj;
	WSkillFieldDamage.colliderObject = p_fieldDamageObj;

	WSkillProjectile.dotweenComponent = p_projectileObj->GetComponent<Dotween>();
	WSkillFieldDamage.dotweenComponent = p_fieldDamageObj->GetComponent<Dotween>();
}

void MagicianSkillSystem::SetWSkillDebugPair(std::pair<GameObject*, float> p_projectileObjectPair, std::pair<GameObject*, float> p_fieldObjectPair)
{
	WSkillProjectile.debugObject = p_projectileObjectPair.first;
	WSkillFieldDamage.debugObject = p_fieldObjectPair.first;

	m_WSkillProjectileRadius = p_projectileObjectPair.second;
	m_WSkillFieldRadius = p_fieldObjectPair.second;
}

void MagicianSkillSystem::Start()
{
	SetOtherComponentsAsMember();

	QSkillProjectile.debugObject->SetSelfActive(false);
	QSkillFieldDamage.debugObject->SetSelfActive(false);
	
	WSkillProjectile.debugObject->SetSelfActive(false);
	WSkillFieldDamage.debugObject->SetSelfActive(false);
}

void MagicianSkillSystem::Update()
{
	PlayerSkillSystem::Update();

	/// Start 문에서 SetActive(false)를 못알아 먹는다.
	if (!isColliderSetActiveFalseSet)
	{
		SetSkillRequirmentsActive(QSkillProjectile, false);
		SetSkillRequirmentsActive(QSkillFieldDamage, false);
		SetSkillRequirmentsActive(WSkillProjectile, false);
		SetSkillRequirmentsActive(WSkillFieldDamage, false);
		isColliderSetActiveFalseSet = true;
	}

	if (isQSkillActivating)
	{
		QSkillProjectile.debugObject->GetTransform()->SetWorldPosition(QSkillProjectile.colliderObject->GetTransform()->GetWorldPosition());
		QSkillFieldDamage.debugObject->GetTransform()->SetWorldPosition(QSkillFieldDamage.colliderObject->GetTransform()->GetWorldPosition());
	}

	if (isWSkillActivating)
	{
		WSkillProjectile.debugObject->GetTransform()->SetWorldPosition(WSkillProjectile.colliderObject->GetTransform()->GetWorldPosition());
		WSkillFieldDamage.debugObject->GetTransform()->SetWorldPosition(WSkillFieldDamage.colliderObject->GetTransform()->GetWorldPosition());
	}
}
