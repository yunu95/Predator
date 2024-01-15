#include "InWanderLand.h"
#include "PlayerSkillSystem.h"
#include "Unit.h"

void PlayerSkillSystem::SkillActivate(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition)
{
    switch (p_currentSkill)
    {
    case Unit::SkillEnum::QSkill:
        QSkillActivate(p_skillPosition);
        break;
    case Unit::SkillEnum::WSkill:
        WSkillActivate(p_skillPosition);
        break;
    case Unit::SkillEnum::ESkill:
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
