#include "InWanderLand.h"
#include "MeleeAttackSystem.h"
#include "UnitProductor.h"
#include "StatusEffect.h"

void MeleeAttackSystem::Attack(Unit* opponentUnit, float offset)
{
	switch (m_meleeAttackType)
	{
		case MeleeAttackType::Collider: 
		{
			meleeAttackColliderObject->GetTransform()->
				SetWorldPosition(ownerUnitObject->GetTransform()->GetWorldPosition() + ownerUnitObject->GetTransform()->GetWorldRotation().Forward() *  -1 * m_unitComponent->GetAttackOffset());
			meleeAttackColliderObject->GetTransform()->SetWorldRotation(ownerUnitObject->GetTransform()->GetWorldRotation());

			meleeAttackColliderObject->SetSelfActive(true);
			meleeAttackColliderDebugObject->SetSelfActive(true);
			//meleeAttackColliderObject->GetComponent<physics::BoxCollider>()->SetActive(true);
			colliderActivated = true;
		}
			break;
		case MeleeAttackType::DirectAttack:
		{
			/// collider 생성이 아닌, Unit->Damaged 함수를 호출하는 단일 공격로직 적용.
			/// 그렇다면 Robin의 경우 패시브는 어떻게 적용시킬 것인가?
			opponentUnit->Damaged(m_unitComponent, m_unitComponent->DetermineAttackDamage(m_unitComponent->GetUnitDamage()));
			if (m_specialEffect != nullptr)
				m_specialEffect->ApplyStatus(m_unitComponent, opponentUnit);
		}
			break;
		default:
			break;
	}
}

void MeleeAttackSystem::SetMeleeAttackType(MeleeAttackType p_type)
{
	m_meleeAttackType = p_type;
}

void MeleeAttackSystem::SetColliderObject(GameObject* colliderObj)
{
	meleeAttackColliderObject = colliderObj;
	meleeAttackColliderObject->SetSelfActive(false);
}

void MeleeAttackSystem::SetColliderDebugObject(GameObject* debugobj)
{
	meleeAttackColliderDebugObject = debugobj;
	meleeAttackColliderDebugObject->SetSelfActive(false);
}

void MeleeAttackSystem::SetOwnerUnitObject(GameObject* unitobj)
{
	ownerUnitObject = unitobj;
}

void MeleeAttackSystem::SetColliderRemainTime(float time)
{
	attackColliderRemainTime = time;
}

void MeleeAttackSystem::SetDirectAttackSpecialEffect(StatusEffect* p_effect)
{
	m_specialEffect = p_effect;
}

void MeleeAttackSystem::SetDamage(float p_dmg)
{
	m_attackDamage = p_dmg;
}

void MeleeAttackSystem::Start()
{
	m_unitComponent = ownerUnitObject->GetComponent<Unit>();

	if (m_meleeAttackType == MeleeAttackType::Collider)
	{

		meleeAttackColliderObject->SetSelfActive(false);
		meleeAttackColliderDebugObject->SetSelfActive(false);
	/*	meleeAttackColliderObject->SetParent(m_unitComponent->GetGameObject());
		meleeAttackColliderDebugObject->SetParent(m_unitComponent->GetGameObject());*/
	}
	//contentsLayer->RegisterToEditorComponentVector(this);

}

void MeleeAttackSystem::Update()
{
	if (m_meleeAttackType == MeleeAttackType::Collider)
	{
		meleeAttackColliderDebugObject->GetTransform()->
			SetWorldPosition(meleeAttackColliderObject->GetTransform()->GetWorldPosition());

		meleeAttackColliderDebugObject->GetTransform()->SetWorldRotation(meleeAttackColliderObject->GetTransform()->GetWorldRotation());

		if (colliderActivated)
		{
			colliderRemainElasped += Time::GetDeltaTime();
			if (colliderRemainElasped >= attackColliderRemainTime)
			{
				colliderRemainElasped = 0.0f;
				meleeAttackColliderObject->SetSelfActive(false);
				meleeAttackColliderDebugObject->SetSelfActive(false);
				//meleeAttackColliderObject->GetComponent<physics::BoxCollider>()->SetActive(false);
				colliderActivated = false;
			}
		}
	}
}
