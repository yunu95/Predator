#include "InWanderLand.h"
#include "WarriorSkillSystem.h"
#include "KnockBackComponent.h"
#include "Dotween.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "TacticModeSystem.h"

void WarriorSkillSystem::ActivateSkillOne(Vector3d skillPos)
{
	QknockBackSkill.colliderObject->GetComponent<KnockBackComponent>()->SkillStarted();
	QknockBackSkill.colliderObject->SetParent(GetGameObject());

	isQSkillReady = false;

	SetSkillRequirmentsActive(QknockBackSkill, true);

	m_unitNavComponent->SetActive(false);

	skillPos = CheckSkillRange(skillPos, Unit::SkillEnum::Q);

	float tempDistance = (skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

	// 목표 위치로 돌진
	m_unitDotween->DOMove(skillPos, tempDistance / m_QskillRushSpeed).OnComplete([=]()
		{
			isOncedActivated = false;

			SetSkillRequirmentsActive(QknockBackSkill, false);

			QknockBackSkill.colliderObject->GetComponent<KnockBackComponent>()->ClearCrushedUnitList();

			m_unitNavComponent->SetActive(true);
			m_unitNavComponent->AssignToNavigationField(m_unitComponent->GetNavField());
			m_unitNavComponent->Relocate(skillPos);
			m_unitComponent->SetUnitStateIdle();
			if (m_unitComponent->isPermittedToTacticAction)
			{
				m_unitComponent->isPermittedToTacticAction = false;
				TacticModeSystem::Instance().ReportTacticActionFinished();
			}
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
