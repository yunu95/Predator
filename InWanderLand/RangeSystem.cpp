#include "InWanderLand.h"
#include "RangeSystem.h"
#include "Unit.h"

void RangeSystem::Start()
{

}

void RangeSystem::OnTriggerEnter(physics::Collider* collider)
{
	/// 설정해준 타입에 따라 Unit에서 호출될 함수를 정해준다.
	// 부모 오브젝트 (유닛) 의 타입이 다르다면 충돌 처리
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		m_unitComponent->GetUnitSide() != colliderUnitComponent->GetUnitSide() /*&&
		!colliderUnitComponent->IsUnitDead()*/)
	{
		m_unitComponent->AddToOpponentObjectList(colliderUnitComponent);
	}
}

void RangeSystem::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		m_unitComponent->GetUnitSide() != colliderUnitComponent->GetUnitSide() &&
		!(m_unitComponent->GetJustCrushedState()))
	{
		m_unitComponent->DeleteFromOpponentObjectList(colliderUnitComponent);
	}
}

void RangeSystem::SetOwnerUnitComponent(Unit* unitComponent)
{
	m_unitComponent = unitComponent;
}


