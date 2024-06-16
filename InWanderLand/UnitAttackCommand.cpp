#include "UnitAttackCommand.h"
#include "SkillPreviewSystem.h"
#include "GlobalConstant.h"


UnitAttackCommand::UnitAttackCommand(Unit* unit, Vector3d expectedPos, Unit* enemyUnit, bool isAttackAfterMove, Vector3d direction)
    : UnitCommand(unit, expectedPos), renderer{ nullptr }, enemyUnit(enemyUnit), direction(direction)
{
    this->commandType = UnitCommand::Attack;
    this->commandCost = application::GlobalConstant::GetSingletonInstance().pod.tacticAttackCost;

    if (!isAttackAfterMove)
    {
        auto playerType = static_cast<PlayerCharacterType::Enum>(unit->GetUnitTemplateData().pod.playerUnitType.enumValue);

        switch (playerType)
        {
        case PlayerCharacterType::Robin:
        {
            if (TacticModeSystem::Instance().robinLastCommand)
            {
                this->expectedPos = TacticModeSystem::Instance().robinLastCommand->GetExpectedPos();
            }
            else
            {
                this->expectedPos = unit->GetGameObject()->GetTransform()->GetWorldPosition();
            }
        }
        break;
        case PlayerCharacterType::Ursula:
        {
            if (TacticModeSystem::Instance().ursulaLastCommand)
            {
                this->expectedPos = TacticModeSystem::Instance().ursulaLastCommand->GetExpectedPos();
            }
            else
            {
                this->expectedPos = unit->GetGameObject()->GetTransform()->GetWorldPosition();
            }
        }
        break;
        case PlayerCharacterType::Hansel:
        {
            if (TacticModeSystem::Instance().hanselLastCommand)
            {
                this->expectedPos = TacticModeSystem::Instance().hanselLastCommand->GetExpectedPos();
            }
            else
            {
                this->expectedPos = unit->GetGameObject()->GetTransform()->GetWorldPosition();
            }
        }
        break;
        }
    }
}

UnitAttackCommand::~UnitAttackCommand()
{

}

yunuGI::ITexture* UnitAttackCommand::GetIconTexture()
{
    static yunuGI::ITexture* iconTexture = graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/NewFolder/Skill_RobinW2.dds");
    return iconTexture;
}

void UnitAttackCommand::Execute()
{
    unit->OrderAttack(enemyUnit->GetWeakPtr<Unit>());
    unit->onAttackHit.AddVolatileCallback([this](std::weak_ptr<Unit>) {isDone = true; });
}

void UnitAttackCommand::ShowPreviewMesh()
{
    if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Robin")
    {
        renderer = SkillPreviewSystem::Instance().ShowAttackImage(SkillPreviewSystem::UnitType::Robin, expectedPos, direction);
    }
    else if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Ursula")
    {
        renderer = SkillPreviewSystem::Instance().ShowAttackImage(SkillPreviewSystem::UnitType::Ursula, expectedPos, direction);
    }
    else if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Hansel")
    {
        renderer = SkillPreviewSystem::Instance().ShowAttackImage(SkillPreviewSystem::UnitType::Hansel, expectedPos, direction);
    }
}

void UnitAttackCommand::HidePreviewMesh()
{
    SkillPreviewSystem::Instance().HideAttackImage(renderer);
}
