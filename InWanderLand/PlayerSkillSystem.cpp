#include "PlayerSkillSystem.h"

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
		case Unit::SkillEnum::E:
			ESkillActivate(p_skillPosition);
			break;
		default:
			break;
	}


}

void PlayerSkillSystem::SetPlayerDotweenComponent(Dotween* p_dotween)
{
	m_UnitDotween = p_dotween;
}
