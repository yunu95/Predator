#include "UnitFactory.h"
#include "Unit.h"
#include "Player.h"
#include "RangeSystem.h"

void UnitFactory::AddUnitComponent(UnitType enumType)
{
	switch (enumType)
	{
		case UnitType::PLAYER:
			m_unit = m_unitGameObject->AddComponent<Player>();
			m_unitGameObject->setName("Player");
			break;

		case UnitType::ENEMY:
			m_unit = m_unitGameObject->AddComponent<Unit>();
			m_unitGameObject->setName("Unit");
			break;

		case UnitType::BOSS:
			//unitGameObject->AddComponent<Boss>();
			break;
	}
}

/// 이 함수는 추후 fbx를 로드할 때 교체될 예정.
void UnitFactory::AddMeshComponent(UnitType enumType)
{
	m_staticMeshRenderer = m_unitGameObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	m_staticMeshRenderer->GetGI().LoadMesh("Capsule");
	m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 1, 0 });
}

void UnitFactory::AddColliderComponent()
{
	m_circleCollider = m_unitGameObject->AddComponent<CircleCollider2D>();
	m_circleCollider->SetRadius(defaultUnitColliderRadius);
}

void UnitFactory::AddNavigationComponent()
{
	m_navigationAgent = m_unitGameObject->AddComponent<NavigationAgent>();
	m_navigationAgent->SetSpeed(2);
	m_navigationAgent->SetRadius(0.5);
}

void UnitFactory::AddRangeSystemComponent()
{
	m_rangeSystem = m_rangeGameObject->AddComponent<RangeSystem>();
	m_rangeSystem->SetUnitComponent(m_unit);
	m_rangeSystem->SetIdRadius(defaultIdRadius);
	m_rangeSystem->SetAtkRadius(defalutAtkRadius);
}

GameObject* UnitFactory::CreateUnit(UnitType enumType)
{

	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	m_rangeGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	AddUnitComponent(enumType);
	AddMeshComponent(enumType);
	AddColliderComponent();
	AddNavigationComponent();
	AddRangeSystemComponent();

	return m_unitGameObject;
}

void UnitFactory::SetUnitColliderRadius(float rad)
{
	m_circleCollider->SetRadius(rad);
}

void UnitFactory::SetRangeRadius(float idRadius, float atkRadius)
{
	m_rangeSystem->SetIdRadius(idRadius);
	m_rangeSystem->SetAtkRadius(atkRadius);
}

void UnitFactory::SetNavField(NavigationField* field)
{
	m_navigationAgent->AssignToNavigationField(field);
}
