#include "InWanderLand.h"
#include "PlayerSkillSystem.h"
#include "Unit.h"

void PlayerSkillSystem::SkillActivate(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition)
{
	switch (p_currentSkill)
	{
		case Unit::SkillEnum::Q:
			QSkillActivate(p_skillPosition);
			break;
		case Unit::SkillEnum::W:
			WSkillActivate(p_skillPosition);
			break;
		default:
			break;
	}
}

void PlayerSkillSystem::SetOtherComponentsAsMember()
{
	m_unitComponent = GetGameObject()->GetComponent<Unit>();
	m_unitDotween = GetGameObject()->GetComponent<Dotween>();
	m_unitNavComponent = GetGameObject()->GetComponent<NavigationAgent>();
}
