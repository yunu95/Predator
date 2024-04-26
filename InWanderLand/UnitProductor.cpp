#include "InWanderLand.h"
#include "UnitProductor.h"
#include "RangeSystem.h"
#include "PlayerController.h"
#include "Dotween.h"
#include "DebugMeshes.h"
#include "Unit_TemplateData.h"
#include "SingleNavigationField.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "GlobalConstant.h"
#include "BurnEffect.h"
#include "PlayerSkillSystem.h"

void UnitProductor::SetUnitComponentMembers()
{
	//m_unitGameObject->GetTransform()->SetWorldPosition(m_startPosition);
	auto frontDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(frontDebugObject, DebugMeshType::Cube, yunuGI::Color::black(), true);
	frontDebugObject->GetTransform()->SetLocalScale({ 0.5, 0.5, 0.5 });
	frontDebugObject->GetTransform()->SetWorldPosition(m_unitGameObject->GetTransform()->GetLocalRotation().Forward() * -1);
	frontDebugObject->SetParent(m_unitGameObject);

	auto unitColliderDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(unitColliderDebugObject, DebugMeshType::Sphere, yunuGI::Color::green(), false);
	unitColliderDebugObject->SetParent(m_unitGameObject);
	unitColliderDebugObject->GetTransform()->SetWorldScale(Vector3d(UNIT_LENGTH, UNIT_LENGTH, UNIT_LENGTH));
	m_unitComponent->SetStaticMeshComponent(unitColliderDebugObject->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>());

	auto burnEffect = m_unitGameObject->AddComponent<BurnEffect>();

	/// Unit Member Setting
	m_unitComponent->GetGameObject()->setName(m_objectName);
	m_unitComponent->SetUnitType(m_unitType);
	m_unitComponent->SetUnitSide(m_unitSide);

	m_unitComponent->SetUnitMaxHp(m_healthPoint);
	m_unitComponent->SetUnitAp(m_autoAttackDamage);

	m_unitComponent->SetAtkRadius(m_atkRadius);
	m_unitComponent->SetIdRadius(m_idRadius);
	m_unitComponent->SetUnitSpeed(m_unitSpeed);

	m_unitComponent->SetAttackDelay(m_attackDelay);
	m_unitComponent->SetAttackTimingFrame(m_attackTimingFrame);

	m_unitComponent->SetMaxAggroNumber(m_maxAggroNumber);

	m_unitComponent->SetFbxName(m_unitFbxName);

	m_unitComponent->SetAttackOffset(m_attackOffset);

	m_unitComponent->m_defensePoint = m_defensePoint;
	m_unitComponent->m_dodgeProbability = m_dodgeProbability;
	m_unitComponent->m_criticalHitMultiplier = m_criticalHitMultiplier;
	m_unitComponent->m_criticalDamageDecreaseMultiplier = m_criticalDamageDecreaseMultiplier;
	m_unitComponent->m_criticalHitProbability = m_criticalHitProbability;

	switch (m_unitType)
	{
		case Unit::UnitType::Warrior:
			m_skillOneEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.playerOneQSkillDelay;
			m_skillOneTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.playerOneQSkillTimingFrame;
			m_skillTwoEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.playerOneWSkillDelay;
			m_skillTwoTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.playerOneWSkillTimingFrame;
			break;
		case Unit::UnitType::Magician:
			m_skillOneEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.playerTwoQSkillDelay;
			m_skillOneTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.playerTwoQSkillTimingFrame;
			m_skillTwoEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.playerTwoWSkillDelay;
			m_skillTwoTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.playerTwoWSkillTimingFrame;
			break;
		case Unit::UnitType::Healer:
			m_skillOneEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.playerThreeQSkillDelay;
			m_skillOneTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.playerThreeQSkillTimingFrame;
			m_skillTwoEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.playerThreeWSkillDelay;
			m_skillTwoTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.playerThreeWSkillTimingFrame;
			break;
		case Unit::UnitType::MeleeEnemy:
			m_skillOneEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.eliteMeleeEnemySkillDelay;
			m_skillOneTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.eliteMeleeEnemySkillTimingFrame;
			break;
		case Unit::UnitType::RangedEnemy:
			m_skillTwoEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.eliteRangedEnemySkillDelay;
			m_skillTwoTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.eliteRangedEnemySkillTimingFrame;
			break;
		case Unit::UnitType::Boss:
			m_skillOneEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.bossSkillOneDelay;
			m_skillOneTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.bossSkillOneTimingFrame;
			m_skillTwoEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.bossSkillTwoDelay;
			m_skillTwoTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.bossSkillTwoTimingFrame;
			m_skillThreeEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.bossSkillThreeDelay;
			m_skillThreeTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.bossSkillThreeTimingFrame;
			m_skillFourEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.bossSkillFourDelay;
			m_skillFourTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.bossSkillFourTimingFrame;
			break;
		case Unit::UnitType::EnemySpawnGate:
			m_skillOneEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.bossSkillOneDelay;
			m_skillOneTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.bossSkillOneTimingFrame;
			break;
		case Unit::UnitType::SpikeTrap:
			m_skillOneEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.spikeTrapSkillDelay;
			m_skillOneTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.spikeTrapSkillTimingFrame;
			break;
		case Unit::UnitType::ChessTrap:
			m_skillOneEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.chessTrapSkillDelay;
			m_skillOneTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.chessTrapSkillTimingFrame;
			break;
		case Unit::UnitType::TriggeredTrap:
			m_skillOneEngageDelay = application::GlobalConstant::GetSingletonInstance().pod.triggeredTrapSkillDelay;
			m_skillOneTimingFrame = application::GlobalConstant::GetSingletonInstance().pod.triggeredTrapSkillTimingFrame;
			break;
		default:
			break;
	}

	if (m_unitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		m_unitComponent->RegisterSkillDuration(Unit::SkillEnum::Q, m_skillOneEngageDelay);
		m_unitComponent->RegisterSkillDuration(Unit::SkillEnum::W, m_skillTwoEngageDelay);
		m_unitComponent->RegisterSkillTimingFrame(Unit::SkillEnum::Q, m_skillOneTimingFrame);
		m_unitComponent->RegisterSkillTimingFrame(Unit::SkillEnum::W, m_skillTwoTimingFrame);
	}
	else if (m_unitComponent->GetUnitSide() == Unit::UnitSide::Enemy)
	{
		m_unitComponent->RegisterSkillDuration(Unit::SkillEnum::BossSkillOne, m_skillOneEngageDelay);
		m_unitComponent->RegisterSkillTimingFrame(Unit::SkillEnum::BossSkillOne, m_skillOneTimingFrame);
		m_unitComponent->RegisterSkillDuration(Unit::SkillEnum::BossSkillTwo, m_skillTwoEngageDelay);
		m_unitComponent->RegisterSkillTimingFrame(Unit::SkillEnum::BossSkillTwo, m_skillTwoTimingFrame);
		m_unitComponent->RegisterSkillDuration(Unit::SkillEnum::BossSkillThree, m_skillThreeEngageDelay);
		m_unitComponent->RegisterSkillTimingFrame(Unit::SkillEnum::BossSkillThree, m_skillThreeTimingFrame);
		m_unitComponent->RegisterSkillDuration(Unit::SkillEnum::BossSkillFour, m_skillFourEngageDelay);
		m_unitComponent->RegisterSkillTimingFrame(Unit::SkillEnum::BossSkillFour, m_skillFourTimingFrame);
	}
}

void UnitProductor::AddRangeSystemComponent() const
{
	auto unitRangeSystemObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	// 2-1. RangeSystem Component
	RangeSystem* rangeSystemComponent = unitRangeSystemObject->AddComponent<RangeSystem>();
	rangeSystemComponent->SetOwnerUnitComponent(m_unitComponent);

	// 2-2. RangeSystem Collider
	auto rangesystemCollider = unitRangeSystemObject->AddComponent<physics::SphereCollider>();
	rangesystemCollider->SetRadius(m_idRadius);
	unitRangeSystemObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	unitRangeSystemObject->SetParent(m_unitGameObject);
}

void UnitProductor::AddColliderComponent() const 
{
	auto unitCollider = m_unitGameObject->AddComponent<physics::SphereCollider>();	// 빈 껍데기에 
	unitCollider->SetRadius(UNIT_LENGTH * 0.5f);
	//m_unitGameObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
}

void UnitProductor::AddNavigationComponent() 
{
	m_navField = &SingleNavigationField::Instance();
	m_unitComponent->m_navAgentComponent = m_unitGameObject->AddComponent<NavigationAgent>();
	m_unitComponent->m_navAgentComponent->AssignToNavigationField(m_navField);
	m_unitComponent->m_navAgentComponent->SetRadius(0.3f);
	m_unitComponent->SetNavField(m_navField);
}

void UnitProductor::AddDotweenComponent() const
{
	/// 6. Dotween 추가
	m_unitComponent->dotween = m_unitGameObject->AddComponent<Dotween>();
	m_unitComponent->knockBackTimer = m_unitGameObject->AddComponent<TimerComponent>();
}

void UnitProductor::SetUnitAnimationFunction()
{
	if (m_unitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		m_unitComponent->RegisterSkillWithAnimation(Unit::SkillEnum::Q);
		m_unitComponent->RegisterSkillWithAnimation(Unit::SkillEnum::W);
	}
	else if (m_unitComponent->GetUnitSide() == Unit::UnitSide::Enemy)
	{
		m_unitComponent->RegisterSkillWithAnimation(Unit::SkillEnum::BossSkillOne);
		m_unitComponent->RegisterSkillWithAnimation(Unit::SkillEnum::BossSkillTwo);
		m_unitComponent->RegisterSkillWithAnimation(Unit::SkillEnum::BossSkillThree);
		m_unitComponent->RegisterSkillWithAnimation(Unit::SkillEnum::BossSkillFour);
	}
}

std::string UnitProductor::GetUnitFBXName() const
{
	return m_unitFbxName;
}

bool UnitProductor::SelectUnitProductorByFbxName(std::string p_name)
{
	return (m_unitFbxName == p_name);
}

void UnitProductor::SetPlayerRelatedComponents()
{
	m_unitComponent->SetPlayerSerialNumber(m_unitType);
	m_unitComponent->SetSkillPreviewType(qSkillPreviewType, wSkillPreviewType);
	PlayerController::SingleInstance().AddPlayerUnit(m_unitComponent);

	float qCoolTimeTemp;
	float eCoolTimeTemp;

	m_unitComponent->SetRessurectMaxCount(application::GlobalConstant::GetSingletonInstance().pod.maxResurrectCount);

	switch (m_unitComponent->GetUnitType())
	{
		case Unit::UnitType::Warrior:
		{
			qCoolTimeTemp = application::GlobalConstant::GetSingletonInstance().pod.robinQSkillCoolTime;
			eCoolTimeTemp = application::GlobalConstant::GetSingletonInstance().pod.robinESkillCoolTime;
		}
			break;
		case Unit::UnitType::Magician:
		{
			qCoolTimeTemp = application::GlobalConstant::GetSingletonInstance().pod.ursulaQSkillCoolTime;
			eCoolTimeTemp = application::GlobalConstant::GetSingletonInstance().pod.ursulaESkillCoolTime;
		}
			break;
		case Unit::UnitType::Healer:
		{
			qCoolTimeTemp = application::GlobalConstant::GetSingletonInstance().pod.hanselQSkillCoolTime;
			eCoolTimeTemp = application::GlobalConstant::GetSingletonInstance().pod.hanselESkillCoolTime;
		}
			break;
	}

	m_unitComponent->GetGameObject()->GetComponent<PlayerSkillSystem>()->SetQSkillCoolTime(qCoolTimeTemp);
	m_unitComponent->GetGameObject()->GetComponent<PlayerSkillSystem>()->SetESkillCoolTime(eCoolTimeTemp);
}

void UnitProductor::MappingUnitData(application::editor::POD_Unit_TemplateData p_podData)
{
	//m_unitType = static_cast<Unit::UnitType>(p_podData.unitType);
	m_healthPoint = p_podData.m_healthPoint;
	m_autoAttackDamage = p_podData.m_autoAttackDamage;
	m_criticalHitProbability = p_podData.m_criticalHitProbability;
	m_criticalHitMultiplier = p_podData.m_criticalHitMultiplier;
	m_defensePoint = p_podData.m_defensePoint;
	m_dodgeProbability = p_podData.m_dodgeProbability;
	m_criticalDamageDecreaseMultiplier = p_podData.m_criticalDamageDecreaseMultiplier;
	m_idRadius = p_podData.m_idRadius;
	m_atkRadius = p_podData.m_atkRadius;
	m_unitSpeed = p_podData.m_unitSpeed;
	m_attackDelay = p_podData.m_attackEngageDelay;
	m_attackTimingFrame = p_podData.m_attackTimingFrame;
	m_attackOffset = p_podData.m_attackOffset;
}

void UnitProductor::Update()
{

}

void UnitProductor::Start()
{
	application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
	//contentsLayer->RegisterToEditorComponentVector(this);
}

