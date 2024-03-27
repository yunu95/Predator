#include "RobinSkillDevelopmentSystem.h"
#include "BleedingComponent.h"
#include "TauntingComponent.h"
#include "WarriorSkillSystem.h"
#include "UIButton.h"
#include "Unit.h"

void RobinSkillDevelopmentSystem::Start()
{
	m_passiveComponent->isActivated = false;
	WarriorSkillSystem* warriorSkillSys = static_cast<WarriorSkillSystem*>(m_ownerSkillSystem);

	/// 등록된 버튼들을 설정해주자
	// 1. 패시브 활성화
	m_topNodeSkillButton[0]->SetButtonClickFunction([=]()
		{
			m_passiveComponent->isActivated = true;
		});

	// 2. 도발 스킬 사용 시 방어력 증가
	m_middleNodeSkillButton[0]->SetButtonClickFunction([=]()
		{
			warriorSkillSys->m_developedFunctionToWSkill = [=]()
				{
					m_targetUnit->m_defensePoint += 100;
				};
		});
}

void RobinSkillDevelopmentSystem::SetRobinPassiveComponent(BleedingComponent* p_passive)
{
	m_passiveComponent = p_passive;
}
