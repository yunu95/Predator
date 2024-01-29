#include "InWanderLand.h"
#include "MeleeAttackSystem.h"

void MeleeAttackSystem::Attack(Unit* opponentUnit)
{
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
