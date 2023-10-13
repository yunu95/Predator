#include "RangeSystem.h"
#include "IRangeAction.h"

void RangeSystem::SetUnitGameObject(GameObject* obj)
{
	m_unit = obj;
	//this->GetGameObject()->SetParent(obj);
}

void RangeSystem::SetRangeType(string p_type)
{
	m_type = p_type;
}

void RangeSystem::OnCollisionEnter2D(const Collision2D& collision)
{
	/// 설정해준 타입에 따라 Unit에서 호출될 함수를 정해준다.
	// 임시 조건 - 부모 게임오브젝트가 다를 경우 충돌 이벤트 발생
	if (GetGameObject()->GetParentGameObject() != collision.m_OtherCollider->GetGameObject()->GetParentGameObject())
	{
		
		if (m_type == "Identification")
			m_unit->GetComponent<IRangeAction>()->IdentifyAction(collision.m_OtherCollider->GetTransform()->GetWorldPosition());

		else if (m_type == "Attack")
			m_unit->GetComponent<IRangeAction>()->AttackAction(collision.m_OtherCollider->GetTransform()->GetWorldPosition());

	}


}

