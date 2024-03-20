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
		m_unitComponent->GetUnitSide() != colliderUnitComponent->GetUnitSide())
	{
		float distance = (collider->GetGameObject()->GetTransform()->GetWorldPosition() - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

		m_unitComponent->AddToOpponentObjectList(colliderUnitComponent);
		//m_unitComponent->IdleTransition();
	}
}

void RangeSystem::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		m_unitComponent->GetUnitSide() != colliderUnitComponent->GetUnitSide() &&
		!(m_unitComponent->GetJustCrushedState()))
	{
		/// 넉백공격 당하면서 Exit불릴 때는 지우면 안되는데...
		/// unit에 멤버를 추가
		m_unitComponent->DeleteFromOpponentObjectList(colliderUnitComponent);
	}
}

void RangeSystem::SetOwnerUnitComponent(Unit* unitComponent)
{
	m_unitComponent = unitComponent;
}


