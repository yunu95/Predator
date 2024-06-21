#include "UnitMoveCommand.h"
#include "SkillPreviewSystem.h"

UnitMoveCommand::UnitMoveCommand(Unit* unit, Vector3d expectedPos, std::vector<Vector3d> path, bool isForAttack)
    : UnitCommand(unit, expectedPos), path(path), isForAttack(isForAttack)
{
    this->commandType = UnitCommand::Move;

    this->CalculateCommandCost();
}

UnitMoveCommand::~UnitMoveCommand()
{
    SkillPreviewSystem::Instance().HideShowMoveEndImage(routeMesh);
    SkillPreviewSystem::Instance().DeleteRouteMesh(routeMesh);
}

yunuGI::ITexture* UnitMoveCommand::GetIconTexture()
{
    static yunuGI::ITexture* robin = graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/NewFolder/Que_Move_Robin.dds");
    static yunuGI::ITexture* ursula = graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/NewFolder/Que_Move_Ursula.dds");
    static yunuGI::ITexture* hansel = graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/NewFolder/Que_Move_Hansel.dds");
    switch (GetPlayerType())
    {
    case PlayerCharacterType::Robin:
    {
        return robin;
    }
    case PlayerCharacterType::Ursula:
    {
        return ursula;
    }
    case PlayerCharacterType::Hansel:
    {
        return hansel;
    }
    }
    return robin;
}

void UnitMoveCommand::Execute()
{
    unit->OrderMove(expectedPos);
    unit->OnStateExitCallback()[UnitBehaviourTree::Move].AddVolatileCallback([&]() {
        this->isDone = true;
        });
}

void UnitMoveCommand::ShowPreviewMesh()
{
    auto playerType = static_cast<PlayerCharacterType::Enum>(unit->GetUnitTemplateData().pod.playerUnitType.enumValue);

    switch (playerType)
    {
    case PlayerCharacterType::Robin:
    {
        this->routeMesh = SkillPreviewSystem::Instance().ShowRoute(SkillPreviewSystem::UnitType::Robin, path);
    }
    break;
    case PlayerCharacterType::Ursula:
    {
        this->routeMesh = SkillPreviewSystem::Instance().ShowRoute(SkillPreviewSystem::UnitType::Ursula, path);
    }
    break;
    case PlayerCharacterType::Hansel:
    {
        this->routeMesh = SkillPreviewSystem::Instance().ShowRoute(SkillPreviewSystem::UnitType::Hansel, path);
    }
    break;
    }

    if (isForAttack)
    {
        SkillPreviewSystem::Instance().HideShowMoveEndImage(this->routeMesh);
    }
}

void UnitMoveCommand::HidePreviewMesh()
{
    if (!isForAttack)
    {
        SkillPreviewSystem::Instance().HideShowMoveEndImage(routeMesh);
    }
    SkillPreviewSystem::Instance().DeleteRouteMesh(routeMesh);
}

void UnitMoveCommand::CalculateCommandCost()
{
    float distance = 0.f;
    for (int i = 0; i < path.size() - 1; ++i)
    {
        auto dist = path[i] - path[i + 1];
        distance += dist.Magnitude();
    }
    this->commandCost = distance * application::GlobalConstant::GetSingletonInstance().pod.tacticMoveCost;
}
