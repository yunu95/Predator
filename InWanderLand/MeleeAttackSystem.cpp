#include "MeleeAttackSystem.h"

void MeleeAttackSystem::Attack(Unit* opponentUnit)
{
	//meleeAttackColliderObject->SetSelfActive(true);
	meleeAttackColliderObject->GetComponent<physics::BoxCollider>()->SetActive(true);
	colliderActivated = true;
}

void MeleeAttackSystem::SetColliderObject(GameObject* colliderObj)
{
	meleeAttackColliderObject = colliderObj;
}

void MeleeAttackSystem::SetColliderRemainTime(float time)
{
	attackColliderRemainTime = time;
}

void MeleeAttackSystem::Start()
{
	//meleeAttackColliderObject->SetSelfActive(false);
}

void MeleeAttackSystem::Update()
{
	if (colliderActivated)
	{
		colliderRemainElasped += Time::GetDeltaTime();
		if (colliderRemainElasped >= attackColliderRemainTime)
		{
			//meleeAttackColliderObject->SetSelfActive(false);
			meleeAttackColliderObject->GetComponent<physics::BoxCollider>()->SetActive(false);
			colliderActivated = false;
		}
	}
}
