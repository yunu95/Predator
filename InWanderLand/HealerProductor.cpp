#include "InWanderLand.h"
#include "HealerProductor.h"
#include "RangedAttackSystem.h"
#include "TauntingComponent.h"
#include "DebugMeshes.h"
#include "HealerSkillSystem.h"
#include "DualCastComponent.h"
#include "SingleNavigationField.h"
#include "SkillPreviewSystem.h"

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

	m_idRadius = 4.0f * UNIT_LENGTH;
	m_atkRadius = 2.5f * UNIT_LENGTH;
	m_unitSpeed = 4.5f;

	m_attackDelay = 1.0f;

	m_navField = &SingleNavigationField::Instance();

	qSkillPreviewType = SkillPreviewMesh::OnlyPath;
	wSkillPreviewType = SkillPreviewMesh::OnlyPath;

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

	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();
	UnitProductor::SetUnitComponentMembers();


#pragma endregion

#pragma region Auto Attack Setting (Including Passive Logic)
	auto magicianAttackSystem = m_unitGameObject->AddComponent<RangedAttackSystem>();
	magicianAttackSystem->SetBulletSpeed(10.0f);
#pragma endregion

#pragma region Q Skill Setting
	auto QSkillFieldObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto fieldDamageComponent = QSkillFieldObject->AddComponent<DamageOnlyComponent>();
	fieldDamageComponent->SetSkillOwnerUnit(m_unitComponent);

	auto QSkillFieldCollider = QSkillFieldObject->AddComponent<physics::SphereCollider>();
	m_QSkillFieldRadius = 2.0f * UNIT_LENGTH;
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
	m_WSkillFieldWidth = 2.0f * UNIT_LENGTH / 2;
	m_WSkillFieldHeight = 4.0f * UNIT_LENGTH / 2;

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

	float skillOneRange = 3.0f * UNIT_LENGTH;
	float skillTwoRange = 5.0f * UNIT_LENGTH;

	healerSkillSystem->SetSkillOneRange(skillOneRange);
	healerSkillSystem->SetSkillTwoRange(skillTwoRange);
#pragma endregion

	UnitProductor::AddRangeSystemComponent();
	UnitProductor::AddColliderComponent();
	UnitProductor::AddNavigationComponent();
	UnitProductor::AddDotweenComponent();
	UnitProductor::SetPlayerRelatedComponents();

	SkillPreviewSystem::Instance().SetDefaultSkillRange(m_unitComponent, Unit::SkillEnum::Q, skillOneRange);
	SkillPreviewSystem::Instance().SetDefaultSkillRange(m_unitComponent, Unit::SkillEnum::E, skillTwoRange);

	auto skinnedMeshRenderer = m_unitGameObject->GetChildren()[0]->GetComponent<yunutyEngine::graphics::SkinnedMesh>();
	auto material = skinnedMeshRenderer->GetGI().GetMaterial();
	auto clonedMaterial = graphics::Renderer::SingleInstance().GetResourceManager()->CloneMaterial(L"Blue", material);
	clonedMaterial->SetColor(yunuGI::Color::blue());
	skinnedMeshRenderer->GetGI().SetMaterial(0, clonedMaterial);

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();
	auto& animList = rsrcManager->GetAnimationList();
	for (auto each : animList)
	{
		if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Idle")
		{
			m_baseUnitAnimations.m_idleAnimation = each;
			m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
			animator->PushAnimation(m_baseUnitAnimations.m_idleAnimation);
			animator->Play(m_baseUnitAnimations.m_idleAnimation);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Walk")
		{
			m_baseUnitAnimations.m_walkAnimation = each;
			m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
			animator->PushAnimation(m_baseUnitAnimations.m_walkAnimation);
		}
		/*else */if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleStart")
		{
			m_baseUnitAnimations.m_attackAnimation = each;
			m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleMode")
		{
			m_baseUnitAnimations.m_paralysisAnimation = each;
			m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
			animator->PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_APose")
		{
			m_baseUnitAnimations.m_deathAnimation = each;
			m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
			animator->PushAnimation(m_baseUnitAnimations.m_deathAnimation);
		}
		/// Skill Animation
		if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleMode")
		{
			each->SetLoop(false);
			animator->PushAnimation(each);
			m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::Q, each);
		}
		if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleMode")
		{
			each->SetLoop(false);
			animator->PushAnimation(each);
			m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::E, each);
		}
		/*else */if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleStart")
		{
			m_baseUnitAnimations.m_battleEngageAnimation = each;
			m_baseUnitAnimations.m_battleEngageAnimation->SetLoop(false);
		}
	}
	m_unitComponent->unitAnimations = m_baseUnitAnimations;
	return m_unitComponent;
}
