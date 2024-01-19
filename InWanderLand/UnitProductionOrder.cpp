#include "InWanderLand.h"
#include "UnitProductionOrder.h"
#include "RangeSystem.h"
#include "PlayerController.h"
#include "Dotween.h"
#include "DebugMeshes.h"

GameObject* UnitProductionOrder::CreateUnitWithOrder()
{
	m_unitGameObject->GetTransform()->SetWorldPosition(m_startPosition);
	m_unitGameObject->GetTransform()->scale = { 1.0f, 1.0f, 1.0f };			// 콜라이더가 붙는 오브젝트 Or 그 오브젝트의 부모 오브젝트는 반드시 scale이 1로 통일 되어야 한다.

	/// 2. RangeSystem Gameobject 및 Component 추가
	auto unitRangeSystemObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//unitRangeSystemObject->GetTransform()->scale = { 1.0f, 1.0f, 1.0f };

	// 2-1. RangeSystem Component
	RangeSystem* rangeSystemComponent = unitRangeSystemObject->AddComponent<RangeSystem>();
	rangeSystemComponent->SetOwnerUnitComponent(m_unitComponent);

	// 2-2. RangeSystem Collider
	auto rangesystemCollider = unitRangeSystemObject->AddComponent<physics::SphereCollider>();
	rangesystemCollider->SetRadius(m_idRadius);
	unitRangeSystemObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	unitRangeSystemObject->SetParent(m_unitGameObject);

	/// 3. Collider Component 추가 - object의 scale은 1,1,1로 통일하기
	//auto unitCollider = m_unitGameObject->AddComponent<physics::BoxCollider>();
	auto unitCollider = m_unitGameObject->AddComponent<physics::SphereCollider>();	// 빈 껍데기에 
	unitCollider->SetRadius(1.0f);

	auto unitColliderDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(unitColliderDebugObject, DebugMeshType::Cube, yunuGI::Color::red(), false);
	unitColliderDebugObject->SetParent(m_unitGameObject);


	/// 4. NavigationAgent Component 추가
	auto unitNavigationComponent = m_unitGameObject->AddComponent<NavigationAgent>();
	unitNavigationComponent->AssignToNavigationField(m_navField);
	unitNavigationComponent->SetRadius(0.3f);

	/// 6. Dotween 추가
	m_unitComponent->dotween = m_unitGameObject->AddComponent<Dotween>();
	m_unitComponent->knockBackTimer = m_unitGameObject->AddComponent<Timer>();

	/// Unit Member Setting
	m_unitComponent->GetGameObject()->setName(m_objectName);
	m_unitComponent->SetUnitType(m_unitType);
	m_unitComponent->SetUnitSide(m_unitSide);

	m_unitComponent->SetUnitHp(m_healthPoint);
	m_unitComponent->SetUnitAp(m_autoAttackDamage);

	m_unitComponent->SetAtkRadius(m_atkRadius);
	m_unitComponent->SetIdRadius(m_idRadius);
	m_unitComponent->SetUnitSpeed(m_unitSpeed);

	m_unitComponent->unitAnimations = m_baseUnitAnimations;

	m_unitComponent->SetAttackDelay(m_attackDelay);

	m_unitComponent->SetNavField(m_navField);

	/// + 플레이어 유닛일 경우 특수 처리
	if (m_unitSide == Unit::UnitSide::Player)
		SetPlayerRelatedComponents(m_unitComponent);

	return m_unitGameObject;
}

void UnitProductionOrder::SetPlayerRelatedComponents(Unit* playerUnit)
{
	playerUnit->SetPlayerSerialNumber();
	PlayerController::GetInstance()->AddPlayerUnit(playerUnit);
}

