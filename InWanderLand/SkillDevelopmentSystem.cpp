#include "SkillDevelopmentSystem.h"
#include "UIButton.h"
#include "PlayerSkillSystem.h"


void SkillDevelopmentSystem::SetOwnerUnit(Unit* p_unit)
{
	m_targetUnit = p_unit;
}

void SkillDevelopmentSystem::AddTopLayerButton(UIButton* p_btn)
{
	m_topNodeSkillButton.push_back(p_btn);
}

void SkillDevelopmentSystem::AddMiddleLayerButton(UIButton* p_btn)
{
	m_middleNodeSkillButton.push_back(p_btn);
}

void SkillDevelopmentSystem::AddLowLayerButton(UIButton* p_btn)
{
	m_buttomNodeSkillButton.push_back(p_btn);
}

void SkillDevelopmentSystem::SetSkillSystemComponent(PlayerSkillSystem* p_skillSys)
{
	m_ownerSkillSystem = p_skillSys;
}
