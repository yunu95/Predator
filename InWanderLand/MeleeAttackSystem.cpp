#include "InWanderLand.h"
#include "MeleeAttackSystem.h"
#include "UnitProductionOrder.h"

void MeleeAttackSystem::Attack(Unit* opponentUnit)
{
	meleeAttackColliderObject->GetTransform()->
		SetWorldPosition(ownerUnitObject->GetTransform()->GetWorldPosition() + ownerUnitObject->GetTransform()->rotation.Forward() * -3);
	meleeAttackColliderObject->GetTransform()->rotation = ownerUnitObject->GetTransform()->rotation;

	meleeAttackColliderObject->SetSelfActive(true);
	meleeAttackColliderDebugObject->SetSelfActive(true);
	//meleeAttackColliderObject->GetComponent<physics::BoxCollider>()->SetActive(true);
	colliderActivated = true;
}

void MeleeAttackSystem::SetColliderObject(GameObject* colliderObj)
{
	meleeAttackColliderObject = colliderObj;
}

void MeleeAttackSystem::SetColliderDebugObject(GameObject* debugobj)
{
	meleeAttackColliderDebugObject = debugobj;
}

void MeleeAttackSystem::SetOwnerUnitObject(GameObject* unitobj)
{
	ownerUnitObject = unitobj;
}

void MeleeAttackSystem::SetColliderRemainTime(float time)
{
	attackColliderRemainTime = time;
}

void MeleeAttackSystem::Start()
{
	meleeAttackColliderObject->SetSelfActive(false);
	meleeAttackColliderDebugObject->SetSelfActive(false);
}

void MeleeAttackSystem::Update()
{
	meleeAttackColliderDebugObject->GetTransform()->
		SetWorldPosition(meleeAttackColliderObject->GetTransform()->GetWorldPosition());

	meleeAttackColliderDebugObject->GetTransform()->rotation = meleeAttackColliderObject->GetTransform()->rotation;

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
