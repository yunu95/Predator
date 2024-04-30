#include "InWanderLand.h"
#include "SpecialEffect.h"
#include "Unit.h"
#include "ContentsLayer.h"
#include "Application.h"

void SpecialEffect::Start()
{
}

void SpecialEffect::PlayFunction()
{

}

void SpecialEffect::StopFunction()
{
	if (GetGameObject()->GetSelfActive())
		GetGameObject()->SetSelfActive(false);
}

void SpecialEffect::SetSkillOwnerUnit(Unit* p_unit)
{
	m_ownerUnit = p_unit;
}

