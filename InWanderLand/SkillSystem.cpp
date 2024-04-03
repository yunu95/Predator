#include "InWanderLand.h"
#include "SkillSystem.h"
#include "Unit.h"
#include "Dotween.h"
#include "YunutyEngine.h"
#include "ContentsLayer.h"
#include "Application.h"

void SkillSystem::SetOtherComponentsAsMember()
{
	m_unitComponent = GetGameObject()->GetComponent<Unit>();
	m_unitDotween = GetGameObject()->GetComponent<Dotween>();
	m_unitNavComponent = GetGameObject()->GetComponent<NavigationAgent>();
}

void SkillSystem::SetSkillRequirmentsActive(SkillRequirements p_requirments, bool p_boolen)
{
	p_requirments.colliderObject->SetSelfActive(p_boolen);
	p_requirments.debugObject->SetSelfActive(p_boolen);
}
