#include "RangeSystem.h"
#include "Unit.h"
#include "MoveDetector.h"

void RangeSystem::Start()
{

}

void RangeSystem::SetUnitComponent(Unit* unitComponent)
{
	m_unitComponent = unitComponent;
}

void RangeSystem::SetIdRadius(float radius)
{
	m_tempIDRadius = radius;
	SetRadius(radius);
}

void RangeSystem::SetAtkRadius(float radius)
{
	m_tempAtkRadius = radius;
}

void RangeSystem::OnCollisionEnter2D(const Collision2D& collision)
{
	/// 설정해준 타입에 따라 Unit에서 호출될 함수를 정해준다.
	// 부모 오브젝트 (유닛) 의 타입이 다르다면 충돌 처리
	if (collision.m_OtherCollider->GetGameObject()->GetComponent<Unit>() != nullptr && 
		m_unitComponent->GetType() != collision.m_OtherCollider->GetGameObject()->GetComponent<Unit>()->GetType())
	{	
		float distance = (collision.m_OtherCollider->GetGameObject()->GetTransform()->GetWorldPosition() - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();
		
		MoveDetector::GetInstance()->SetTargetUnit(collision.m_OtherCollider->GetGameObject()->GetComponent<Unit>());
		MoveDetector::GetInstance()->SetChaserUnit(collision.m_OtherCollider->GetGameObject()->GetComponent<Unit>(),
			m_unitComponent);

		m_unitComponent->SetIdRadius(m_tempIDRadius);
		m_unitComponent->SetAtkRadius(m_tempAtkRadius);
		
		m_unitComponent->SetOpponentGameObject(collision.m_OtherCollider->GetGameObject());
		m_unitComponent->EnterIDRange();
		m_unitComponent->IdleTransition();
	}
}

/// <summary>
/// 인식 범위에 있다가 벗어 났을 경우. 즉 
/// </summary>
/// <param name="collision"></param>
void RangeSystem::OnCollisionExit2D(const Collision2D& collision)
{
	if (collision.m_OtherCollider->GetGameObject()->GetComponent<Unit>() != nullptr &&
		m_unitComponent->GetType() != collision.m_OtherCollider->GetGameObject()->GetComponent<Unit>()->GetType())
	{
		m_unitComponent->InitFSM();
	}
}

