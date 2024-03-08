#include "InWanderLand.h"
#include "UnitProductor.h"
#include "RangeSystem.h"
#include "PlayerController.h"
#include "Dotween.h"
#include "DebugMeshes.h"

void UnitProductor::SetCommonComponents()
{
	//m_unitGameObject->GetTransform()->SetWorldPosition(m_startPosition);

	/// 2. RangeSystem Gameobject 및 Component 추가
	auto unitRangeSystemObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	// 2-1. RangeSystem Component
	RangeSystem* rangeSystemComponent = unitRangeSystemObject->AddComponent<RangeSystem>();
	rangeSystemComponent->SetOwnerUnitComponent(m_unitComponent);

	// 2-2. RangeSystem Collider
	auto rangesystemCollider = unitRangeSystemObject->AddComponent<physics::SphereCollider>();
	rangesystemCollider->SetRadius(m_idRadius);
	unitRangeSystemObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	unitRangeSystemObject->SetParent(m_unitGameObject);

	/// 3. Collider Component 추가
	auto unitCollider = m_unitGameObject->AddComponent<physics::SphereCollider>();	// 빈 껍데기에 
	unitCollider->SetRadius(lengthUnit);

	auto unitColliderDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(unitColliderDebugObject, DebugMeshType::Sphere, yunuGI::Color::green(), false);
	unitColliderDebugObject->SetParent(m_unitGameObject);
	unitColliderDebugObject->GetTransform()->SetWorldScale(Vector3d(lengthUnit * 2, lengthUnit * 2, lengthUnit * 2));
	m_unitComponent->SetStaticMeshComponent(unitColliderDebugObject->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>());

	auto frontDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(frontDebugObject, DebugMeshType::Cube, yunuGI::Color::black(), true);
	frontDebugObject->GetTransform()->SetLocalScale( { 0.5, 0.5, 0.5 });
	frontDebugObject->GetTransform()->SetWorldPosition(m_unitGameObject->GetTransform()->GetLocalRotation().Forward());
	frontDebugObject->SetParent(m_unitGameObject);


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

	m_unitComponent->SetMaxAggroNumber(m_maxAggroNumber);

	/// + 플레이어 유닛일 경우 특수 처리
	if (m_unitSide == Unit::UnitSide::Player)
		SetPlayerRelatedComponents(m_unitComponent);
}

void UnitProductor::SetPlayerRelatedComponents(Unit* playerUnit)
{
	playerUnit->SetPlayerSerialNumber(m_unitType);
	playerUnit->SetSkillPreviewType(qSkillPreviewType, wSkillPreviewType);
	PlayerController::GetInstance()->AddPlayerUnit(playerUnit);
}

void UnitProductor::PushWaveData(Vector3d startPos, float delay)
{
	m_waveDelayQueue.push({ startPos, delay });
	if (!isWaveUnitCreated)
	{
		m_duration = delay;
		isWaveUnitCreated = true;
	}
}

void UnitProductor::Update()
{
	if (isWaveTimerStarted && !(m_waveDelayQueue.empty()))
	{
		m_elapsed += Time::GetDeltaTime();

		if (m_elapsed >= m_duration)
		{
			CreateUnit(m_waveDelayQueue.front().first);
			m_waveDelayQueue.pop();
			m_elapsed = 0.0f;

			if (m_waveDelayQueue.empty())
				isWaveTimerStarted = false;
			else
				m_duration = m_waveDelayQueue.front().second;
		}
	}
}

void UnitProductor::Start()
{
	/// 원래는 플레이어 유닛이 들어오면 ok지만 우선은 실행 시 wave Trigger On
	isWaveTimerStarted = true;
}

