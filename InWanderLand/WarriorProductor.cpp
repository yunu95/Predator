#include "InWanderLand.h"
#include "WarriorProductor.h"
#include "BleedingComponent.h"
#include "KnockBackComponent.h"
#include "MeleeAttackSystem.h"
#include "WarriorSkillSystem.h"
#include "TauntingComponent.h"
#include "DebugMeshes.h"
#include "HealerProductor.h"
#include "MagicianProductor.h"
#include "SingleNavigationField.h"
#include "UnitData.h"
#include "PlayerUnit.h"
#include "RobinSkillDevelopmentSystem.h"
#include "SkillPreviewSystem.h"

void WarriorProductor::SetUnitData()
{
    m_objectName = "Warrior";
    m_unitType = Unit::UnitType::Warrior;
    m_unitSide = Unit::UnitSide::Player;

    m_maxHealth = 250;
    m_manaPoint = 100;

    m_autoAttackDamage = 10;
    m_criticalHitProbability = 0.2f;
    m_criticalHitMultiplier = 1.5f;

    m_defensePoint = 15;
    m_dodgeProbability = 0.2f;
    m_criticalDamageDecreaseMultiplier = 0.2f;

    m_maxAggroNumber = 2;

    m_idRadius = 4.0f * UNIT_LENGTH;
    m_atkRadius = 1.7f * UNIT_LENGTH;
    m_unitSpeed = 4.5f;

    m_attackDelay = 1.0f;

    m_navField = &SingleNavigationField::Instance();

    m_unitFbxName = "SKM_Robin";
}

void WarriorProductor::SingletonInitializer()
{
    SetUnitData();
}

Unit* WarriorProductor::CreateUnit(Vector3d startPos)
{
#pragma region Animation Related Member Setting
    m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Robin");
    m_unitGameObject->GetTransform()->SetWorldPosition(startPos);

    /// UnitComponent 추가
    m_unitComponent = m_unitGameObject->AddComponent<PlayerUnit>();
    RobinSkillDevelopmentSystem::Instance().SetOwnerUnit(m_unitComponent);

    UnitProductor::SetUnitComponentMembers();
#pragma endregion

#pragma region Auto Attack Setting (Including Passive Logic)
    auto bleedingSystemObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto bleedingSystem = bleedingSystemObject->AddComponent<BleedingComponent>();
    auto warriorAttackSystem = m_unitGameObject->AddComponent<MeleeAttackSystem>();

    RobinSkillDevelopmentSystem::Instance().SetRobinPassiveComponent(bleedingSystem);

    warriorAttackSystem->SetMeleeAttackType(MeleeAttackType::DirectAttack);
    warriorAttackSystem->SetOwnerUnitObject(m_unitGameObject);
    warriorAttackSystem->SetDirectAttackSpecialEffect(bleedingSystem);
    warriorAttackSystem->SetDamage(m_autoAttackDamage);
#pragma endregion

#pragma region Q Skill Setting
    m_QSkillRadius = 3.0f * UNIT_LENGTH;

    auto qSkillKnockBackObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

    auto qSkillColliderComponent = qSkillKnockBackObject->AddComponent<physics::SphereCollider>();
    qSkillColliderComponent->SetRadius(m_QSkillRadius);
    qSkillKnockBackObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);

    auto qSkillColliderDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    AttachDebugMesh(qSkillColliderDebugObject, DebugMeshType::Sphere, yunuGI::Color::red(), true);
    qSkillColliderDebugObject->GetTransform()->SetLocalScale({ m_QSkillRadius * 2, m_QSkillRadius * 2, m_QSkillRadius * 2 });

    auto knockBackComponent = qSkillKnockBackObject->AddComponent<KnockBackComponent>();
    knockBackComponent->SetSkillOwnerUnit(m_unitComponent);
#pragma endregion

#pragma region W Skill Setting
    m_WSkillRadius = 3.0f * UNIT_LENGTH;

    auto wSkillColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto wSkillColliderComponent = wSkillColliderObject->AddComponent<physics::SphereCollider>();
    wSkillColliderComponent->SetRadius(m_WSkillRadius);
    wSkillColliderObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
    auto wSkillDamageComponent = wSkillColliderObject->AddComponent<TauntingComponent>();
    wSkillDamageComponent->SetSkillOwnerUnit(m_unitComponent);
    wSkillDamageComponent->SetSkillDamage(10.0f);

    auto wSkillColliderDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    AttachDebugMesh(wSkillColliderDebugObject, DebugMeshType::Sphere, yunuGI::Color::green(), true);
    wSkillColliderDebugObject->GetTransform()->SetLocalScale({ m_WSkillRadius * 2, m_WSkillRadius * 2, m_WSkillRadius * 2 });
#pragma endregion
    // warrior SkillSystem
    auto warriorSkillSystem = m_unitGameObject->AddComponent<WarriorSkillSystem>();
    warriorSkillSystem->SetQSkillKnockBackObject(qSkillKnockBackObject);
    warriorSkillSystem->SetWSkillObject(wSkillColliderObject);
    warriorSkillSystem->SetKnockBackDebugObject(qSkillColliderDebugObject, m_QSkillRadius);
    warriorSkillSystem->SetWSkillDebugObject(wSkillColliderDebugObject, m_WSkillRadius);
    warriorSkillSystem->qSkillRadialOverlay = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)
        ->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Overlay);
    warriorSkillSystem->qSkillCooltimeNumberUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)
        ->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Cooltime_Number);
    warriorSkillSystem->eSkillRadialOverlay = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)
        ->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Overlay);
    warriorSkillSystem->eSkillCooltimeNumberUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)
        ->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Cooltime_Number);

    float warriorSkillOneRange = 5.0f * UNIT_LENGTH;
    warriorSkillSystem->SetSkillOneRange(warriorSkillOneRange);
    warriorSkillSystem->SetSkillTwoRange(0.0f);

    RobinSkillDevelopmentSystem::Instance().SetSkillSystemComponent(warriorSkillSystem);

    UnitProductor::AddRangeSystemComponent();
    UnitProductor::AddColliderComponent();
    UnitProductor::AddNavigationComponent();
    UnitProductor::AddDotweenComponent();
    UnitProductor::SetPlayerRelatedComponents();

    //SkillPreviewSystem::Instance().SetDefaultSkillRange(m_unitComponent, Unit::SkillEnum::Q, warriorSkillOneRange);
    //SkillPreviewSystem::Instance().SetDefaultSkillRange(m_unitComponent, Unit::SkillEnum::W, 0.0f);

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
        else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Attack")
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
        else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Death")
        {
            m_baseUnitAnimations.m_deathAnimation = each;
            m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
            animator->PushAnimation(m_baseUnitAnimations.m_deathAnimation);
        }
        /// Skill Animation
        else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Skill1")
        {
            each->SetLoop(true);
            animator->PushAnimation(each);
            m_baseUnitAnimations.m_skillOneAnimation = each;
            m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::Q, each);
        }
        else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Skill2")
        {
            each->SetLoop(true);
            animator->PushAnimation(each);
            m_baseUnitAnimations.m_skillTwoAnimation = each;
            m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::W, each);
        }
        else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleStart")
        {
            m_baseUnitAnimations.m_battleEngageAnimation = each;
            m_baseUnitAnimations.m_battleEngageAnimation->SetLoop(false);
        }
    }
    m_unitComponent->unitAnimations = m_baseUnitAnimations;
    SetUnitSkillFunctionToAnimation();

    return m_unitComponent;
}

// 전략 패턴을 설명하기 위한 예시 코드
// 전략 패턴의 핵심은 객체의 동작을 전략 객체를 통해 바꿀 수 있다는 것.
// 아래의 경우 UnitProductionOrder가 전략에 해당되며, 이 전략 객체가 바뀜에 따라 MakeSome 동작이 바뀐다.
//class Someone
//{
//	// 아래가 전략!
//	UnitProductionOrder* currentStrategy;
//	void MakeSome()
//	{
//		currentStrategy->CreateUnitWithOrder();
//	}
//	void ChangeStrategy(UnitProductionOrder* newStrategy)
//	{
//		currentStrategy = newStrategy;
//	}
//	static void SampleCode()
//	{
//		Someone someDude;
//		someDude.ChangeStrategy(new WarriorProduction);
//		someDude.MakeSome();
//		someDude.ChangeStrategy(new MagicianProduction);
//		someDude.MakeSome();
//		someDude.ChangeStrategy(new HealerProduction);
//		someDude.MakeSome();
//	}
//};