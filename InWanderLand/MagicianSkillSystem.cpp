#include "InWanderLand.h"
#include "MagicianSkillSystem.h"
#include "ParalysisFieldComponent.h"
#include "Dotween.h"
#include "TacticModeSystem.h"
#include "BlindFieldComponent.h"

void MagicianSkillSystem::ActivateSkillOne(Vector3d skillPos)
{
    m_QSkillComponent->m_blindPersistTime = application::GlobalConstant::GetSingletonInstance().pod.ursulaQSkillBlindDuration;
    m_QSkillComponent->m_fieldDamageDelay = application::GlobalConstant::GetSingletonInstance().pod.ursulaQSkillFieldDamageInterval;
    m_QSkillComponent->m_fieldDamage = application::GlobalConstant::GetSingletonInstance().pod.ursulaQSkillFieldDamagePerTick;
    m_QSkillComponent->m_blindPersistTime = application::GlobalConstant::GetSingletonInstance().pod.ursulaQSkillBlindDuration;
    /// 장판의 타이머를 Activate하는 함수가 필요...
    //m_QSkillComponent->ActivateFieldTimer();
    if (m_QRadiusCollider.expired())
    {
        m_QRadiusCollider = QSkillFieldDamage.colliderObject->GetComponentWeakPtr<physics::SphereCollider>();
    }
    if (!m_QRadiusCollider.expired())
    {
        m_QRadiusCollider.lock()->SetRadius(application::GlobalConstant::GetSingletonInstance().pod.ursulaQSkillRadius);
    }
    isQSkillActivating = true;

    QSkillProjectile.colliderObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
    QSkillProjectile.colliderObject->GetTransform()->SetWorldRotation(Quaternion(Vector3d::zero));
    QSkillProjectile.debugObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());

    RotateProjectile(QSkillProjectile.colliderObject, skillPos);
    SetSkillRequirmentsActive(QSkillProjectile, true);

    skillPos = CheckSkillRange(skillPos, Unit::SkillEnum::Q);
    QSkillFieldDamage.colliderObject->GetTransform()->SetWorldPosition(skillPos);			// 오브젝트만 움직여도 collider와 debug는 따라올 것이다.

    float tempDistance = (skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

    QSkillProjectile.dotweenComponent->DOMove(skillPos, tempDistance / application::GlobalConstant::GetSingletonInstance().pod.ursulaQSkillProjectileSpeed).OnComplete([=]()
        {
            isOncedActivated = false;

            SetSkillRequirmentsActive(QSkillProjectile, false);
            SetSkillRequirmentsActive(QSkillFieldDamage, true);
            m_unitComponent->SetUnitStateIdle();

            if (m_unitComponent->isPermittedToTacticAction)
            {
                m_unitComponent->isPermittedToTacticAction = false;
                TacticModeSystem::Instance().ReportTacticActionFinished();
            }

            QSkillFieldDamage.dotweenComponent->DONothing(application::GlobalConstant::GetSingletonInstance().pod.ursulaQSkillFieldDuration).OnComplete([=]()
                {
                    SetSkillRequirmentsActive(QSkillFieldDamage, false);
                    isQSkillActivating = false;
                });
        });
}

void MagicianSkillSystem::ActivateSkillTwo(Vector3d skillPos)
{
    isWSkillActivating = true;
    if (m_ERadiusCollider.expired())
    {
        m_ERadiusCollider = WSkillFieldDamage.colliderObject->GetComponentWeakPtr<physics::SphereCollider>();
    }
    if (!m_ERadiusCollider.expired())
    {
        m_ERadiusCollider.lock()->SetRadius(application::GlobalConstant::GetSingletonInstance().pod.ursulaESkillRadius);
    }
    WSkillProjectile.colliderObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
    WSkillProjectile.debugObject->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());

    SetSkillRequirmentsActive(WSkillProjectile, true);

    skillPos = CheckSkillRange(skillPos, Unit::SkillEnum::W);
    WSkillFieldDamage.colliderObject->GetTransform()->SetWorldPosition(skillPos);			// 오브젝트만 움직여도 collider와 debug는 따라올 것이다.
    float tempDistance = (skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

    WSkillProjectile.dotweenComponent->DOMove(skillPos, tempDistance / m_WSkillProjectileSpeed).OnComplete([=]()
        {
            isOncedActivated = false;

            SetSkillRequirmentsActive(WSkillProjectile, false);
            SetSkillRequirmentsActive(WSkillFieldDamage, true);

            m_unitComponent->SetUnitStateIdle();

            if (m_unitComponent->isPermittedToTacticAction)
            {
                m_unitComponent->isPermittedToTacticAction = false;
                TacticModeSystem::Instance().ReportTacticActionFinished();
            }

            WSkillFieldDamage.dotweenComponent->DONothing(m_WSkillFieldRemainTime).OnComplete([=]()
                {
                    SetSkillRequirmentsActive(WSkillFieldDamage, false);
                    isWSkillActivating = false;
                });
        });
}

void MagicianSkillSystem::SetSkillRequirmentLocalTimeScale(float p_scale)
{
    if (QSkillProjectile.dotweenComponent)
        LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(QSkillProjectile.dotweenComponent, p_scale);
    if (QSkillFieldDamage.dotweenComponent)
        LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(QSkillFieldDamage.dotweenComponent, p_scale);
    if (WSkillProjectile.dotweenComponent)
        LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(WSkillProjectile.dotweenComponent, p_scale);
    if (WSkillFieldDamage.dotweenComponent)
        LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(WSkillFieldDamage.dotweenComponent, p_scale);
}

void MagicianSkillSystem::SetInterActionComponent(BlindFieldComponent* p_QSkillComponent, ParalysisFieldComponent* p_WSkillComponent)
{
    m_QSkillComponent = p_QSkillComponent;
    m_WSkillComponent = p_WSkillComponent;
}

void MagicianSkillSystem::SetQSkillObject(GameObject* p_projectileObj, GameObject* p_fieldDamageObj)
{
    QSkillProjectile.colliderObject = p_projectileObj;
    QSkillFieldDamage.colliderObject = p_fieldDamageObj;

    QSkillProjectile.dotweenComponent = p_projectileObj->GetComponent<Dotween>();
    QSkillFieldDamage.dotweenComponent = p_fieldDamageObj->GetComponent<Dotween>();
}

void MagicianSkillSystem::SetQSkillDebugPair(std::pair<GameObject*, float> p_projectileObjectPair, std::pair<GameObject*, float> p_fieldObjectPair)
{
    QSkillProjectile.debugObject = p_projectileObjectPair.first;
    QSkillFieldDamage.debugObject = p_fieldObjectPair.first;

    m_QSkillProjectileRadius = p_projectileObjectPair.second;
    m_QSkillFieldRadius = p_fieldObjectPair.second;
}


void MagicianSkillSystem::SetWSkillObject(GameObject* p_projectileObj, GameObject* p_fieldDamageObj)
{
    WSkillProjectile.colliderObject = p_projectileObj;
    WSkillFieldDamage.colliderObject = p_fieldDamageObj;

    WSkillProjectile.dotweenComponent = p_projectileObj->GetComponent<Dotween>();
    WSkillFieldDamage.dotweenComponent = p_fieldDamageObj->GetComponent<Dotween>();
}

void MagicianSkillSystem::SetWSkillDebugPair(std::pair<GameObject*, float> p_projectileObjectPair, std::pair<GameObject*, float> p_fieldObjectPair)
{
    WSkillProjectile.debugObject = p_projectileObjectPair.first;
    WSkillFieldDamage.debugObject = p_fieldObjectPair.first;

    m_WSkillProjectileRadius = p_projectileObjectPair.second;
    m_WSkillFieldRadius = p_fieldObjectPair.second;
}

void MagicianSkillSystem::Start()
{
    SetOtherComponentsAsMember();

    SetSkillRequirmentsActive(QSkillProjectile, false);
    SetSkillRequirmentsActive(QSkillFieldDamage, false);

    SetSkillRequirmentsActive(WSkillProjectile, false);
    SetSkillRequirmentsActive(WSkillFieldDamage, false);
}

void MagicianSkillSystem::Update()
{
    PlayerSkillSystem::Update();

    if (isQSkillActivating)
    {
        QSkillProjectile.debugObject->GetTransform()->SetWorldPosition(QSkillProjectile.colliderObject->GetTransform()->GetWorldPosition());
        QSkillFieldDamage.debugObject->GetTransform()->SetWorldPosition(QSkillFieldDamage.colliderObject->GetTransform()->GetWorldPosition());
    }

    if (isWSkillActivating)
    {
        WSkillProjectile.debugObject->GetTransform()->SetWorldPosition(WSkillProjectile.colliderObject->GetTransform()->GetWorldPosition());
        WSkillFieldDamage.debugObject->GetTransform()->SetWorldPosition(WSkillFieldDamage.colliderObject->GetTransform()->GetWorldPosition());
    }
}
