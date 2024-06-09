#include "UnitAttackCommand.h"
#include "SkillPreviewSystem.h"

UnitAttackCommand::UnitAttackCommand(Unit* unit, Vector3d expectedPos)
    : UnitCommand(unit, expectedPos), renderer{ nullptr }
{

}

UnitAttackCommand::~UnitAttackCommand()
{

}

void UnitAttackCommand::Execute()
{
    //unit->OrderAttack(unit->GetWeakPtr<Unit>());
    unit->onAttackHit.AddVolatileCallback([this]() {isDone = true; });
}

void UnitAttackCommand::ShowPreviewMesh()
{
    if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Robin")
    {
        renderer = SkillPreviewSystem::Instance().ShowAttackImage(SkillPreviewSystem::UnitType::Robin, targetPos);
    }
    else if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Ursula")
    {
        renderer = SkillPreviewSystem::Instance().ShowAttackImage(SkillPreviewSystem::UnitType::Ursula, targetPos);
    }
    else if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Hansel")
    {
        renderer = SkillPreviewSystem::Instance().ShowAttackImage(SkillPreviewSystem::UnitType::Hansel, targetPos);
    }
}

void UnitAttackCommand::HidePreviewMesh()
{
    SkillPreviewSystem::Instance().HideAttackImage(renderer);
}
