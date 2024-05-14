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
    m_unitNavComponent = m_unitComponent->m_navAgentComponent;
}

void SkillSystem::SetSkillRequirmentsActive(SkillRequirements p_requirments, bool p_boolen)
{
    p_requirments.colliderObject->SetSelfActive(p_boolen);
    p_requirments.debugObject->SetSelfActive(p_boolen);
}

void SkillSystem::SetSkillRequirmentsPosition(SkillRequirements p_requirments, Vector3d p_pos)
{
    p_requirments.colliderObject->GetTransform()->SetWorldPosition(p_pos);
    p_requirments.debugObject->GetTransform()->SetWorldPosition(p_pos);
}

void SkillSystem::SetSkillRequirmentsRotation(SkillRequirements p_requirments, Quaternion p_quat)
{
    p_requirments.colliderObject->GetTransform()->SetWorldRotation(p_quat);
    p_requirments.debugObject->GetTransform()->SetWorldRotation(p_quat);
}
