//#include "InWanderLand.h"
//#include "MagicianProductor.h"
//#include "RangedAttackSystem.h"
//#include "MagicianSkillSystem.h"
//#include "ParalysisFieldComponent.h"
//#include "BlindFieldComponent.h"
//#include "PlayerUnit.h"
//#include "DebugMeshes.h"
//#include "SingleNavigationField.h"
//#include "UIManager.h"
//
//void MagicianProductor::SetUnitData()
//{
//    m_objectName = "Magician";
//    m_unitType = Unit::UnitType::Magician;
//    m_unitSide = Unit::UnitSide::Player;
//
//    m_maxHealth = 150;
//    m_manaPoint = 100;
//
//    m_autoAttackDamage = 15;
//    m_criticalHitProbability = 0.5f;
//    m_criticalHitMultiplier = 1.5f;
//
//    m_defensePoint = 5;
//    m_dodgeProbability = 0.05f;
//    m_criticalDamageDecreaseMultiplier = 0.05f;
//
//    m_maxAggroNumber = 1;
//
//    m_idRadius = 10.0f * UNIT_LENGTH;
//    m_atkRadius = 3.5f * UNIT_LENGTH;
//    m_unitSpeed = 4.5f;
//
//    m_attackDelay = 5.0f;
//
//    m_navField = &SingleNavigationField::Instance();
//
//    m_unitFbxName = "SKM_Ursula";
//}
//
//void MagicianProductor::SingletonInitializer()
//{
//    //graphics::Renderer::SingleInstance().GetResourceManager()->LoadFile("FBX/Boss");
//    SetUnitData();
//}
//Unit* MagicianProductor::CreateUnit(Vector3d startPos)
//{
//#pragma region Animation Related Member Setting
//    m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Ursula");
//    m_unitGameObject->GetTransform()->SetWorldPosition(startPos);
//
//    /// UnitComponent 추가
//    m_unitComponent = m_unitGameObject->AddComponent<PlayerUnit>();
//    UnitProductor::SetUnitComponentMembers();
//#pragma endregion
//
//#pragma region Auto Attack Setting (Including Passive Logic)
//    auto magicianAttackSystem = m_unitGameObject->AddComponent<RangedAttackSystem>();
//    magicianAttackSystem->SetBulletSpeed(10.0f);
//#pragma endregion
//
//#pragma region Q Skill Setting
//    auto QSkillProjectileObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Fork");
//
//    QSkillProjectileObject->AddComponent<Dotween>();
//
//    auto QSkillProjectileCollider = QSkillProjectileObject->AddComponent<physics::SphereCollider>();
//    m_QSkillProjectileRadius = 1.0f * UNIT_LENGTH;
//    QSkillProjectileCollider->SetRadius(m_QSkillProjectileRadius);
//
//    auto QSkillProjectileDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
//    AttachDebugMesh(QSkillProjectileDebugObject, DebugMeshType::Sphere, yunuGI::Color::red(), false);
//    QSkillProjectileDebugObject->GetTransform()->SetLocalScale({ m_QSkillProjectileRadius * 2 , m_QSkillProjectileRadius * 2 , m_QSkillProjectileRadius * 2 });
//
//    auto QSkillFieldObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
//    QSkillFieldObject->AddComponent<Dotween>();
//    auto fieldDamageComponent = QSkillFieldObject->AddComponent<BlindFieldComponent>();
//    fieldDamageComponent->SetSkillOwnerUnit(m_unitComponent);
//    auto QSkillFieldCollider = QSkillFieldObject->AddComponent<physics::SphereCollider>();
//    m_QSkillFieldRadius = 2.0f * UNIT_LENGTH;
//    QSkillFieldCollider->SetRadius(m_QSkillFieldRadius);
//    auto QSkillFieldDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
//    AttachDebugMesh(QSkillFieldDebugObject, DebugMeshType::Sphere, yunuGI::Color::white(), false);
//    QSkillFieldDebugObject->GetTransform()->SetLocalScale({ m_QSkillFieldRadius * 2, m_QSkillFieldRadius * 2 , m_QSkillFieldRadius * 2 });
//#pragma endregion
//
//#pragma region W Skill Setting
//    auto WSkillProjectileObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
//    WSkillProjectileObject->AddComponent<Dotween>();
//
//    auto WSkillProjectileCollider = WSkillProjectileObject->AddComponent<physics::SphereCollider>();
//    m_WSkillProjectileRadius = 1.0f * UNIT_LENGTH;
//    WSkillProjectileCollider->SetRadius(m_WSkillProjectileRadius);
//
//    auto WSkillProjectileDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
//    AttachDebugMesh(WSkillProjectileDebugObject, DebugMeshType::Sphere, yunuGI::Color::green(), false);
//    WSkillProjectileDebugObject->GetTransform()->SetLocalScale({ m_WSkillProjectileRadius * 2 , m_WSkillProjectileRadius * 2, m_WSkillProjectileRadius * 2 });
//
//    auto WSkillFieldObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
//    WSkillFieldObject->AddComponent<Dotween>();
//    auto WfieldDamageComponent = WSkillFieldObject->AddComponent<ParalysisFieldComponent>();
//    WfieldDamageComponent->SetSkillOwnerUnit(m_unitComponent);
//    auto WSkillFieldCollider = WSkillFieldObject->AddComponent<physics::SphereCollider>();
//    WfieldDamageComponent->m_collider = WSkillFieldCollider;
//
//    m_WSkillFieldRadius = application::GlobalConstant::GetSingletonInstance().pod.ursulaESkillRadius;
//    WSkillFieldCollider->SetRadius(m_WSkillFieldRadius);
//    auto WSkillFieldDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
//    AttachDebugMesh(WSkillFieldDebugObject, DebugMeshType::Sphere, yunuGI::Color::blue(), true);
//    WSkillFieldDebugObject->GetTransform()->SetLocalScale({ m_WSkillFieldRadius * 2 , m_WSkillFieldRadius * 2 , m_WSkillFieldRadius * 2 });
//#pragma endregion
//
//#pragma region SkillSystem Setting
//    auto magicianSkillSystem = m_unitGameObject->AddComponent<MagicianSkillSystem>();
//
//    magicianSkillSystem->SetQSkillDebugPair({ QSkillProjectileDebugObject, m_QSkillProjectileRadius }, { QSkillFieldDebugObject, m_QSkillFieldRadius });
//    magicianSkillSystem->SetQSkillObject(QSkillProjectileObject, QSkillFieldObject);
//
//    magicianSkillSystem->SetWSkillDebugPair({ WSkillProjectileDebugObject, m_WSkillProjectileRadius }, { WSkillFieldDebugObject, m_WSkillFieldRadius });
//    magicianSkillSystem->SetWSkillObject(WSkillProjectileObject, WSkillFieldObject);
//
//    magicianSkillSystem->qSkillRadialOverlay = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)
//        ->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Overlay);
//    magicianSkillSystem->qSkillCooltimeNumberUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)
//        ->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Cooltime_Number);
//    magicianSkillSystem->eSkillRadialOverlay = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)
//        ->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Overlay);
//    magicianSkillSystem->eSkillCooltimeNumberUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)
//        ->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Cooltime_Number);
//
//    float skillOneRange = 6.0f * UNIT_LENGTH;
//    float skillTwoRange = 6.0f * UNIT_LENGTH;
//
//    magicianSkillSystem->SetSkillOneRange(skillOneRange);
//    magicianSkillSystem->SetSkillTwoRange(skillTwoRange);
//    magicianSkillSystem->m_QSkillComponent = fieldDamageComponent;
//#pragma endregion
//
//    UnitProductor::AddRangeSystemComponent();
//    UnitProductor::AddColliderComponent();
//    UnitProductor::AddNavigationComponent();
//    UnitProductor::AddDotweenComponent();
//    UnitProductor::SetPlayerRelatedComponents();
//
//    auto skinnedMeshRenderer = m_unitGameObject->GetChildren()[0]->GetComponent<yunutyEngine::graphics::SkinnedMesh>();
//    auto material = skinnedMeshRenderer->GetGI().GetMaterial();
//    auto clonedMaterial = graphics::Renderer::SingleInstance().GetResourceManager()->CloneMaterial(L"Red", material);
//    clonedMaterial->SetColor(yunuGI::Color::red());
//    skinnedMeshRenderer->GetGI().SetMaterial(0, clonedMaterial);
//
//    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
//    auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();
//    auto& animList = rsrcManager->GetAnimationList();
//    for (auto each : animList)
//    {
//        if (each->GetName() == L"Ani_Ursula_Idle")
//        {
//            m_baseUnitAnimations.m_idleAnimation = each;
//            m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
//            animator->PushAnimation(m_baseUnitAnimations.m_idleAnimation);
//            animator->Play(m_baseUnitAnimations.m_idleAnimation);
//        }
//        else if (each->GetName() == L"Ani_Ursula_Walk")
//        {
//            m_baseUnitAnimations.m_walkAnimation = each;
//            m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
//            animator->PushAnimation(m_baseUnitAnimations.m_walkAnimation);
//        }
//        else if (each->GetName() == L"Ani_Ursula_Attack")
//        {
//            m_baseUnitAnimations.m_attackAnimation = each;
//            m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
//        }
//		//else if (each->GetName() == L"Rig_Robin|Ani_Ursula_APose")
//		//{
//		//	m_baseUnitAnimations.m_paralysisAnimation = each;
//		//	m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
//		//	animator->PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
//		//}
//        else if (each->GetName() == L"Ani_Ursula_Death")
//        {
//            m_baseUnitAnimations.m_deathAnimation = each;
//            m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
//            animator->PushAnimation(m_baseUnitAnimations.m_deathAnimation);
//        }
//        /// Skill Animation
//        else if (each->GetName() == L"Ani_Ursula_Skill1")
//        {
//            each->SetLoop(false);
//            animator->PushAnimation(each);
//            m_baseUnitAnimations.m_skillOneAnimation = each;
//            m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::Q, each);
//        }
//        else if (each->GetName() == L"Ani_Ursula_Skill2")
//        {
//            each->SetLoop(true);
//            animator->PushAnimation(each);
//            m_baseUnitAnimations.m_skillTwoAnimation = each;
//            m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::W, each);
//        }
//        else if (each->GetName() == L"Ani_Ursula_Live")
//        {
//            m_baseUnitAnimations.m_battleEngageAnimation = each;
//            m_baseUnitAnimations.m_battleEngageAnimation->SetLoop(false);
//        }
//    }
//    //for (auto each : animList)
//    //{
//    //    if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Idle")
//    //    {
//    //        m_baseUnitAnimations.m_idleAnimation = each;
//    //        m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
//    //        animator->PushAnimation(m_baseUnitAnimations.m_idleAnimation);
//    //        animator->Play(m_baseUnitAnimations.m_idleAnimation);
//    //    }
//    //    else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Walk")
//    //    {
//    //        m_baseUnitAnimations.m_walkAnimation = each;
//    //        m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
//    //        animator->PushAnimation(m_baseUnitAnimations.m_walkAnimation);
//    //    }
//    //    /*else */if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Attack")
//    //    {
//    //        m_baseUnitAnimations.m_attackAnimation = each;
//    //        m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
//    //    }
//    //    else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleMode")
//    //    {
//    //        m_baseUnitAnimations.m_paralysisAnimation = each;
//    //        m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
//    //        animator->PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
//    //    }
//    //    else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Death")
//    //    {
//    //        m_baseUnitAnimations.m_deathAnimation = each;
//    //        m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
//    //        animator->PushAnimation(m_baseUnitAnimations.m_deathAnimation);
//    //    }
//    //    /// Skill Animation
//    //    else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Skill1")
//    //    {
//    //        each->SetLoop(true);
//    //        animator->PushAnimation(each);
//    //        m_baseUnitAnimations.m_skillOneAnimation = each;
//    //        m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::Q, each);
//    //    }
//    //    /*else */if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Skill2")
//    //    {
//    //        each->SetLoop(true);
//    //        animator->PushAnimation(each);
//    //        m_baseUnitAnimations.m_skillTwoAnimation = each;
//    //        m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::W, each);
//    //    }
//    //    /*else */if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleStart")
//    //    {
//    //        m_baseUnitAnimations.m_battleEngageAnimation = each;
//    //        m_baseUnitAnimations.m_battleEngageAnimation->SetLoop(false);
//    //    }
//    //}
//    m_unitComponent->unitAnimations = m_baseUnitAnimations;
//    SetUnitSkillFunctionToAnimation();
//
//    return m_unitComponent;
//}
