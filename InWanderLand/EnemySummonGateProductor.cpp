//#include "EnemySummonGateProductor.h"
//#include "SingleNavigationField.h"
//#include "EnemySummonSkillSystem.h"
//
//void EnemySummonGateProductor::SetUnitData()
//{
//	m_objectName = "EnemySpawnGate";
//	m_unitType = Unit::UnitType::EnemySpawnGate;
//	m_unitSide = Unit::UnitSide::Enemy;
//
//	m_maxHealth = 1;
//	m_manaPoint = 100;
//
//	m_autoAttackDamage = 15;
//	m_criticalHitProbability = 0.5f;
//	m_criticalHitMultiplier = 1.5f;
//
//	m_defensePoint = 5;
//	m_dodgeProbability = 0.05f;
//	m_criticalDamageDecreaseMultiplier = 0.05f;
//
//	m_maxAggroNumber = 3;
//
//	m_idRadius = 0.0f * UNIT_LENGTH;
//	m_atkRadius = 0.0f * UNIT_LENGTH;
//	m_unitSpeed = 4.5f;
//
//	m_attackDelay = 5.0f;
//
//	m_navField = &SingleNavigationField::Instance();
//
//	m_unitFbxName = "SKM_Robin";
//}
//
//void EnemySummonGateProductor::SingletonInitializer()
//{
//	SetUnitData();
//}
//
//Unit* EnemySummonGateProductor::CreateUnit(Vector3d startPos)
//{
//#pragma region Animation Related Member Setting
//	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Robin");
//	m_unitGameObject->GetTransform()->SetWorldPosition(startPos);
//
//	/// UnitComponent 추가
//	m_unitComponent = m_unitGameObject->AddComponent<Unit>();
//	UnitProductor::SetUnitComponentMembers();
//
//#pragma endregion
//
//	auto gateSkillSystem = m_unitGameObject->AddComponent<EnemySummonSkillSystem>();
//
//	/// rangeSystem만 제외한다면 Boss와 다름 없는 유닛.
//	/// chase, move, attack 없고, 주기적으로 skill만 사용하는 유닛이다.
//
//	if (isDamagedUnit)
//	{
//		UnitProductor::AddColliderComponent();
//	}
//
//
//	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
//	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();
//	auto& animList = rsrcManager->GetAnimationList();
//	for (auto each : animList)
//	{
//		if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Idle")
//		{
//			m_baseUnitAnimations.m_idleAnimation = each;
//			m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
//			animator->PushAnimation(m_baseUnitAnimations.m_idleAnimation);
//			animator->Play(m_baseUnitAnimations.m_idleAnimation);
//		}
//		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Walk")
//		{
//			m_baseUnitAnimations.m_walkAnimation = each;
//			m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
//			animator->PushAnimation(m_baseUnitAnimations.m_walkAnimation);
//		}
//		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleStart")
//		{
//			m_baseUnitAnimations.m_attackAnimation = each;
//			m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
//			animator->PushAnimation(m_baseUnitAnimations.m_attackAnimation);
//		}
//		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleMode")
//		{
//			m_baseUnitAnimations.m_paralysisAnimation = each;
//			m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
//			animator->PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
//		}
//		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_APose")
//		{
//			m_baseUnitAnimations.m_deathAnimation = each;
//			m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
//			animator->PushAnimation(m_baseUnitAnimations.m_deathAnimation);
//		}
//		/// Skill Animation
//		if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleMode")
//		{
//			each->SetLoop(false);
//			animator->PushAnimation(each);
//			m_baseUnitAnimations.m_skillOneAnimation = each;
//			m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::BossSkillOne, each);
//		}
//	}
//	m_unitComponent->unitAnimations = m_baseUnitAnimations;
//	SetUnitSkillFunctionToAnimation();
//
//	return m_unitComponent;
//}
//
//void EnemySummonGateProductor::SetUnitCanBeDamaged(bool p_boolen)
//{
//	isDamagedUnit = p_boolen;
//}
