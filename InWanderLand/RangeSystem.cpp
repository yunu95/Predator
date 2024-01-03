#include "RangeSystem.h"
#include "Unit.h"

void RangeSystem::Start()
{

}

void RangeSystem::OnTriggerEnter(physics::Collider* collider)
{
	/// 설정해준 타입에 따라 Unit에서 호출될 함수를 정해준다.
// 부모 오브젝트 (유닛) 의 타입이 다르다면 충돌 처리
	if (collider->GetGameObject()->GetComponent<Unit>() != nullptr &&
		m_unitComponent->GetUnitSide() != collider->GetGameObject()->GetComponent<Unit>()->GetUnitSide())
	{
		float distance = (collider->GetGameObject()->GetTransform()->GetWorldPosition() - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

		m_unitComponent->AddToOpponentObjectList(collider->GetGameObject());
		//m_unitComponent->IdleTransition();
	}
}

void RangeSystem::OnTriggerExit(physics::Collider* collider)
{
	if (collider->GetGameObject()->GetComponent<Unit>() != nullptr &&
		m_unitComponent->GetUnitSide() != collider->GetGameObject()->GetComponent<Unit>()->GetUnitSide())
	{
		m_unitComponent->DeleteFromOpponentObjectList(collider->GetGameObject());
	}
}

void RangeSystem::SetOwnerUnitComponent(Unit* unitComponent)
{
	m_unitComponent = unitComponent;
}


