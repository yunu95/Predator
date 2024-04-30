#include "InWanderLand.h"
#include "StatusEffect.h"
#include "Unit.h"
#include "ContentsLayer.h"
#include "Application.h"

void StatusEffect::Start()
{
}

void StatusEffect::PlayFunction()
{

}

void StatusEffect::StopFunction()
{
	if (!GetGameObject()->GetComponentWeakPtr<StatusEffect>().expired())
	{
		yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
	}
}

void StatusEffect::SetSkillOwnerUnit(Unit* p_unit)
{
	m_ownerUnit = p_unit;
}

