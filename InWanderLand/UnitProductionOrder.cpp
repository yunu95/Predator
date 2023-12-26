#include "UnitProductionOrder.h"
#include "RangeSystem.h"
#include "PlayerController.h"

GameObject* UnitProductionOrder::CreateUnitWithOrder()
{

	/// 1. UnitGameObject 생성

	// 1-1. unitType에 따른 Unit 컴포넌트 추가
	Unit* m_unitComponent = m_unitGameObject->AddComponent<Unit>();

	/// 2. RangeSystem Gameobject 추가
	auto unitRangeSystemObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	unitRangeSystemObject->SetParent(m_unitGameObject);
	unitRangeSystemObject->GetTransform()->SetWorldRotation(Vector3d(90, 0, 0));

	// 2-1. RangeSystem Component 추가
	RangeSystem* rangeSystemComponent = unitRangeSystemObject->AddComponent<RangeSystem>();
	rangeSystemComponent->SetOwnerUnitComponent(m_unitComponent);
	//// 2-2. RangeSystem Debug Mesh 추가
	//auto rangeSystemMesh = unitRangeSystemObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();

	/// 3. Collider Component 추가
	auto unitColliderComponent = m_unitGameObject->AddComponent<CircleCollider2D>();
	//unitColliderComponent->SetRadius(1.0f);

	m_unitComponent->GetGameObject()->setName(m_objectName);
	m_unitComponent->SetUnitType(m_unitType);
	m_unitComponent->SetUnitSide(m_unitSide);

	m_unitComponent->SetUnitHp(m_hp);
	m_unitComponent->SetUnitAp(m_ap);

	m_unitComponent->SetAtkRadius(m_atkRadius);
	m_unitComponent->SetIdRadius(m_idRadius);
	m_unitComponent->SetUnitSpeed(m_unitSpeed);

	rangeSystemComponent->SetRadius(m_idRadius);

	unitRangeSystemObject->GetTransform()->scale = Vector2d(m_idRadius * 2, m_idRadius * 2);

	m_unitComponent->SetIdleAnimation(m_idleAnimation);
	m_unitComponent->SetWalkAnimation(m_walkAnimation);
	m_unitComponent->SetAttackAnimation(m_attackAnimation);
	m_unitComponent->SetDeathAnimation(m_deathAnimation);

	if (m_unitSide == Unit::UnitSide::Player)
		SetPlayerRelatedComponents(m_unitComponent);

	return m_unitGameObject;
}

void UnitProductionOrder::SetPlayerRelatedComponents(Unit* playerUnit)
{
	playerUnit->SetPlayerSerialNumber();
	PlayerController::GetInstance()->AddPlayerUnit(playerUnit);
}

