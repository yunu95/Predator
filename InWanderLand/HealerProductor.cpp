#include "InWanderLand.h"
#include "HealerProductor.h"
#include "RangedAttackSystem.h"
#include "TauntingComponent.h"
#include "DebugMeshes.h"
#include "HealerSkillSystem.h"
#include "DualCastComponent.h"
#include "SingleNavigationField.h"

void HealerProductor::SetUnitData()
{
	m_objectName = "Healer";
	m_unitType = Unit::UnitType::Healer;
	m_unitSide = Unit::UnitSide::Player;

	m_healthPoint = 120;
	m_manaPoint = 100;

	m_autoAttackDamage = 8;
	m_criticalHitProbability = 0.1f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 5;
	m_dodgeProbability = 0.05f;
	m_criticalDamageDecreaseMultiplier = 0.05f;

	m_maxAggroNumber = 1;

	m_idRadius = 4.0f * lengthUnit;
	m_atkRadius = 2.5f * lengthUnit;
	m_unitSpeed = 4.5f;

	m_attackDelay = 1.0f;

	m_navField = &SingleNavigationField::Instance();

	qSkillPreviewType = SkillPreviewSystem::SkillPreviewMesh::OnlyPath;
	wSkillPreviewType = SkillPreviewSystem::SkillPreviewMesh::OnlyPath;

	m_unitFbxName = "SKM_Robin";
}

void HealerProductor::SingletonInitializer()
{
	//graphics::Renderer::SingleInstance().GetResourceManager()->LoadFile("FBX/Boss");
	SetUnitData();
}

Unit* HealerProductor::CreateUnit(Vector3d startPos)
{
#pragma region Animation Related Member Setting
	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Robin");
	m_unitGameObject->GetTransform()->SetWorldPosition(startPos);

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();
	auto& animList = rsrcManager->GetAnimationList();
	for (auto each : animList)
	{
		if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Idle")
		{
			m_baseUnitAnimations.m_idleAnimation = each;
			m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_idleAnimation);
			animator->GetGI().Play(m_baseUnitAnimations.m_idleAnimation);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Walk")
		{
			m_baseUnitAnimations.m_walkAnimation = each;
			m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_walkAnimation);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleStart")
		{
			m_baseUnitAnimations.m_attackAnimation = each;
			m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_attackAnimation);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleMode")
		{
			m_baseUnitAnimations.m_paralysisAnimation = each;
			m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_APose")
		{
			m_baseUnitAnimations.m_deathAnimation = each;
			m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_deathAnimation);
		}
	}
#pragma endregion

	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();

#pragma region Auto Attack Setting (Including Passive Logic)
	auto magicianAttackSystem = m_unitGameObject->AddComponent<RangedAttackSystem>();
	magicianAttackSystem->SetBulletSpeed(10.0f);
#pragma endregion

#pragma region Q Skill Setting
	auto QSkillFieldObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto fieldDamageComponent = QSkillFieldObject->AddComponent<DamageOnlyComponent>();
	fieldDamageComponent->SetSkillOwnerUnit(m_unitComponent);

	auto QSkillFieldCollider = QSkillFieldObject->AddComponent<physics::SphereCollider>();
	m_QSkillFieldRadius = 2.0f * lengthUnit;
	QSkillFieldCollider->SetRadius(m_QSkillFieldRadius);
	QSkillFieldObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);

	auto QSkillFieldDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(QSkillFieldDebugObject, DebugMeshType::Sphere)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::white(), true));
	//QSkillFieldDebugObject->GetTransform()->scale = { pow(m_QSkillFieldRadius, 2), pow(m_QSkillFieldRadius, 2) , pow(m_QSkillFieldRadius, 2) };
	QSkillFieldDebugObject->GetTransform()->SetWorldScale({ m_QSkillFieldRadius * 2, m_QSkillFieldRadius * 2, m_QSkillFieldRadius * 2 });
#pragma endregion

#pragma region W Skill Setting
	auto WSkillFieldObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto dualCastComponent = WSkillFieldObject->AddComponent<DualCastComponent>();
	dualCastComponent->SetSkillOwnerUnit(m_unitComponent);

	auto WSkillFieldCollider = WSkillFieldObject->AddComponent<physics::BoxCollider>();
	m_WSkillFieldWidth = 2.0f * lengthUnit / 2;
	m_WSkillFieldHeight = 4.0f * lengthUnit / 2;

	WSkillFieldCollider->SetHalfExtent({ m_WSkillFieldWidth / 2.0f, 0.1, m_WSkillFieldHeight / 2.0f });
	WSkillFieldObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);

	auto WSkillFieldDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(WSkillFieldDebugObject, DebugMeshType::Cube)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::blue(), true));
	WSkillFieldDebugObject->GetTransform()->SetLocalScale({ m_WSkillFieldWidth, 1, m_WSkillFieldHeight });

#pragma endregion

#pragma region SkillSystem Setting
	auto healerSkillSystem = m_unitGameObject->AddComponent<HealerSkillSystem>();

	healerSkillSystem->SetQSkillObject(QSkillFieldObject);
	healerSkillSystem->SetQSkillDebugInfo(QSkillFieldDebugObject);

	healerSkillSystem->SetWSkillObject(WSkillFieldObject);
	healerSkillSystem->SetWSkillDebugInfo(WSkillFieldDebugObject);
#pragma endregion

	UnitProductor::AddRangeSystemComponent();
	UnitProductor::AddColliderComponent();
	UnitProductor::AddNavigationComponent();
	UnitProductor::AddDotweenComponent();
	UnitProductor::SetUnitComponentMembers();
	UnitProductor::SetPlayerRelatedComponents(m_unitComponent);

	return m_unitComponent;
}
