#include "MagicianSkillSystem.h"
#include "Dotween.h"

void MagicianSkillSystem::SetSkillRequirmentsActive(SkillRequirements p_requirments, bool p_boolen)
{
	p_requirments.skillCollider->SetActive(p_boolen);
	p_requirments.colliderObject->SetSelfActive(p_boolen);
	p_requirments.debugObject->SetSelfActive(p_boolen);
}

void MagicianSkillSystem::QSkillActivate(Vector3d skillPos)
{
	/// 장판의 타이머를 Activate하는 함수가 필요...
	//m_QSkillComponent->ActivateFieldTimer();

	isQSkillActivating = true;

	QSkillProjectile.colliderObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());	

	SetSkillRequirmentsActive(QSkillProjectile, true);

	QSkillFieldDamage.colliderObject->GetTransform()->SetWorldPosition(skillPos);			// 오브젝트만 움직여도 collider와 debug는 따라올 것이다.

	float tempDistance = (skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();
	QSkillProjectile.dotweenComponent->DOMove(skillPos, tempDistance / m_QSkillProjectileSpeed).OnComplete([=]()
		{
			SetSkillRequirmentsActive(QSkillProjectile, false);
			SetSkillRequirmentsActive(QSkillFieldDamage, true);

			QSkillFieldDamage.dotweenComponent->DONothing(m_QSkillFieldRemainTime).OnComplete([=]()
				{
					SetSkillRequirmentsActive(QSkillFieldDamage, false);
				});
		});
}

void MagicianSkillSystem::WSkillActivate(Vector3d skillPos)
{

}

void MagicianSkillSystem::ESkillActivate(Vector3d skillPos)
{

}

void MagicianSkillSystem::SetInterActionComponent(BlindFieldComponent* p_QSkillComponent, ParalysisFieldComponent* p_WSkillComponent)
{
	m_QSkillComponent = p_QSkillComponent;
	m_WSkillComponent = p_WSkillComponent;
}

void MagicianSkillSystem::SetQSkillCollider(physics::SphereCollider* p_projectileCollider, physics::SphereCollider* p_fieldDamageCollider)
{
	QSkillProjectile.skillCollider = p_projectileCollider;
	QSkillFieldDamage.skillCollider = p_fieldDamageCollider;
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

void MagicianSkillSystem::Start()
{
	QSkillProjectile.debugObject->GetTransform()->scale = { pow(m_QSkillProjectileRadius, 2), pow(m_QSkillProjectileRadius, 2), pow(m_QSkillProjectileRadius, 2) };
	QSkillProjectile.debugObject->SetSelfActive(false);

	QSkillFieldDamage.debugObject->GetTransform()->scale = { pow(m_QSkillFieldRadius, 2), pow(m_QSkillFieldRadius, 2), pow(m_QSkillFieldRadius, 2) };
	QSkillFieldDamage.debugObject->SetSelfActive(false);
}

void MagicianSkillSystem::Update()
{
	if (!isColliderSetActiveFalseSet)
	{
		SetSkillRequirmentsActive(QSkillProjectile, false);
		SetSkillRequirmentsActive(QSkillFieldDamage, false);
		isColliderSetActiveFalseSet = true;
	}

	if (isQSkillActivating)
	{
		QSkillProjectile.debugObject->GetTransform()->SetWorldPosition(QSkillProjectile.colliderObject->GetTransform()->GetWorldPosition());
		QSkillFieldDamage.debugObject->GetTransform()->SetWorldPosition(QSkillFieldDamage.colliderObject->GetTransform()->GetWorldPosition());
	}
}
