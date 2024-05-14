#include "InWanderLand.h"
#include "WarriorSkillSystem.h"
#include "KnockBackComponent.h"
#include "Dotween.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "TacticModeSystem.h"

Component::Coroutine WarriorSkillSystem::ImpactOnTheEnd()
{
    GetKnockBackComponent()->ClearCrushedUnitList();
    GetKnockBackComponent()->GetTriggerCollider().lock()->SetRadius(application::GlobalConstant::GetSingletonInstance().pod.robinQSkillImpactKnockbackRadius);
    GetKnockBackComponent()->m_damage = application::GlobalConstant::GetSingletonInstance().pod.robinQSkillDamageImpact;
    GetKnockBackComponent()->pushDistance = application::GlobalConstant::GetSingletonInstance().pod.robinQSkillImpactKnockbackDistance;
    GetKnockBackComponent()->pushDuration = application::GlobalConstant::GetSingletonInstance().pod.robinQSkillImpactKnockbackDuration;
    co_await std::suspend_always{};
    SetSkillRequirmentsActive(QknockBackSkill, false);
    if (m_unitComponent->isPermittedToTacticAction)
    {
        m_unitComponent->isPermittedToTacticAction = false;
        TacticModeSystem::Instance().ReportTacticActionFinished();
    }
}
KnockBackComponent* WarriorSkillSystem::GetKnockBackComponent()
{
    if (!m_QSkillKncokBackComponent)
    {
        m_QSkillKncokBackComponent = QknockBackSkill.colliderObject->GetComponent<KnockBackComponent>();
    }
    return m_QSkillKncokBackComponent;
}
void WarriorSkillSystem::ActivateSkillOne(Vector3d skillPos)
{
    GetKnockBackComponent()->SkillStarted();
    GetKnockBackComponent()->GetTriggerCollider().lock()->SetRadius(application::GlobalConstant::GetSingletonInstance().pod.robinQSkillRushKnockbackRadius);
    GetKnockBackComponent()->m_damage = application::GlobalConstant::GetSingletonInstance().pod.robinQSkillDamageRush;
    GetKnockBackComponent()->pushDistance = application::GlobalConstant::GetSingletonInstance().pod.robinQSkillRushKnockbackDistance;
    GetKnockBackComponent()->pushDuration = application::GlobalConstant::GetSingletonInstance().pod.robinQSkillRushKnockbackDuration;
    QknockBackSkill.colliderObject->SetParent(GetGameObject());
    QknockBackSkill.colliderObject->GetTransform()->SetLocalPosition(Vector3d::zero);

    isQSkillReady = false;

    SetSkillRequirmentsActive(QknockBackSkill, true);

    m_unitNavComponent->SetActive(false);

    skillPos = CheckSkillRange(skillPos, Unit::SkillEnum::Q);

    float tempDistance = (skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

    m_QskillRushSpeed = application::GlobalConstant::GetSingletonInstance().pod.robinQSkillRushSpeed;
    // 목표 위치로 돌진
    m_unitDotween->DOMove(skillPos, tempDistance / m_QskillRushSpeed).OnComplete([=]()
        {
            isOncedActivated = false;

            GetKnockBackComponent()->ClearCrushedUnitList();

            m_unitNavComponent->SetActive(true);
            m_unitNavComponent->AssignToNavigationField(m_unitComponent->GetNavField());
            m_unitNavComponent->Relocate(skillPos);
            m_unitComponent->SetUnitStateIdle();
            GetKnockBackComponent()->GetGameObject()->SetParent(Scene::getCurrentScene());
            GetKnockBackComponent()->GetTransform()->SetWorldPosition(skillPos);
            StartCoroutine(ImpactOnTheEnd());
        });
}

void WarriorSkillSystem::ActivateSkillTwo(Vector3d skillPos)
{
    isESkillReady = false;

    SetSkillRequirmentsActive(WTauntSkill, true);

    m_unitDotween->DONothing(m_wSkillColliderRemainTime).OnComplete([=]()
        {
            isOncedActivated = false;
            SetSkillRequirmentsActive(WTauntSkill, false);
            TacticModeSystem::Instance().ReportTacticActionFinished();
            m_unitComponent->SetUnitStateIdle();
            if (m_unitComponent->isPermittedToTacticAction)
            {
                m_unitComponent->isPermittedToTacticAction = false;
                TacticModeSystem::Instance().ReportTacticActionFinished();
            }
        });
    m_developedFunctionToWSkill();
}

void WarriorSkillSystem::SetQSkillKnockBackObject(GameObject* obj)
{
    QknockBackSkill.colliderObject = obj;
}

void WarriorSkillSystem::SetWSkillObject(GameObject* obj)
{
    WTauntSkill.colliderObject = obj;
}

void WarriorSkillSystem::SetKnockBackDebugObject(GameObject* obj, float radius)
{
    QknockBackSkill.debugObject = obj;
    m_QSkillRadius = radius;
}

void WarriorSkillSystem::SetWSkillDebugObject(GameObject* obj, float radius)
{
    WTauntSkill.debugObject = obj;
    m_WSkillRadius = radius;
}

void WarriorSkillSystem::SetSkillRequirmentLocalTimeScale(float p_scale)
{
    if (QknockBackSkill.dotweenComponent)
        LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(QknockBackSkill.dotweenComponent, p_scale);
    if (WTauntSkill.dotweenComponent)
        LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(WTauntSkill.dotweenComponent, p_scale);
}

void WarriorSkillSystem::Start()
{
    SetOtherComponentsAsMember();

    SetSkillRequirmentsActive(QknockBackSkill, false);
    SetSkillRequirmentsActive(WTauntSkill, false);

    QknockBackSkill.colliderObject->SetParent(GetGameObject());
    QknockBackSkill.debugObject->SetParent(GetGameObject());
    WTauntSkill.colliderObject->SetParent(GetGameObject());
    WTauntSkill.debugObject->SetParent(GetGameObject());

    m_developedFunctionToWSkill = []() {};
}

void WarriorSkillSystem::Update()
{
    PlayerSkillSystem::Update();
}
