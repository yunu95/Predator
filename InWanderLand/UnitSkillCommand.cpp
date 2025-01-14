#include "UnitSkillCommand.h"
#include "SkillPreviewSystem.h"
#include "TacticModeSystem.h"
#include "RobinChargeSkill.h"
#include "RobinTauntSkill.h"
#include "UrsulaBlindSkill.h"
#include "UrsulaParalysisSkill.h"
#include "HanselChargeSkill.h"
#include "HanselProjectileSkill.h"


UnitSkillCommand::UnitSkillCommand(Unit* unit, Vector3d expectedPos, SkillType::Enum skillType)
    : UnitCommand(unit, expectedPos), renderer{ nullptr }, skillType(skillType)
{
    this->commandType = UnitCommand::Skill;
    this->skillTargetPos = expectedPos;
    switch (skillType)
    {
    case SkillType::ROBIN_Q:
    {
        this->commandCost = RobinChargeSkill::pod.cost;
        if (TacticModeSystem::Instance().robinLastCommand)
        {
            this->previewStartPos = TacticModeSystem::Instance().robinLastCommand->GetExpectedPos();
            previewStartPos.y = unit->GetGameObject()->GetTransform()->GetWorldPosition().y;
        }
        else
        {
            this->previewStartPos = unit->GetGameObject()->GetTransform()->GetWorldPosition();
        }
    }
    break;
    case SkillType::ROBIN_W:
    {
        this->commandCost = RobinTauntSkill::pod.skillCost;
        if (TacticModeSystem::Instance().robinLastCommand)
        {
            this->previewStartPos = TacticModeSystem::Instance().robinLastCommand->GetExpectedPos();
            previewStartPos.y = unit->GetGameObject()->GetTransform()->GetWorldPosition().y;
            this->expectedPos = TacticModeSystem::Instance().robinLastCommand->GetExpectedPos();
        }
        else
        {
            this->previewStartPos = unit->GetGameObject()->GetTransform()->GetWorldPosition();
            this->expectedPos = unit->GetGameObject()->GetTransform()->GetWorldPosition();
        }
    }
    break;
    case SkillType::URSULA_Q:
    {
        this->commandCost = UrsulaBlindSkill::pod.skillCost;
        if (TacticModeSystem::Instance().ursulaLastCommand)
        {
            this->previewStartPos = TacticModeSystem::Instance().ursulaLastCommand->GetExpectedPos();
            previewStartPos.y = unit->GetGameObject()->GetTransform()->GetWorldPosition().y;
            this->expectedPos = TacticModeSystem::Instance().ursulaLastCommand->GetExpectedPos();
        }
        else
        {
            this->previewStartPos = unit->GetGameObject()->GetTransform()->GetWorldPosition();
            this->expectedPos = unit->GetGameObject()->GetTransform()->GetWorldPosition();
        }
    }
    break;
    case SkillType::URSULA_W:
    {
        this->commandCost = UrsulaParalysisSkill::pod.skillCost;
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
    case SkillType::HANSEL_Q:
    {
        this->commandCost = HanselChargeSkill::pod.skillCost;
    }
    break;
    case SkillType::HANSEL_W:
    {
        this->commandCost = HanselProjectileSkill::pod.skillCost;
        if (TacticModeSystem::Instance().hanselLastCommand)
        {
            this->previewStartPos = TacticModeSystem::Instance().hanselLastCommand->GetExpectedPos();
            previewStartPos.y = unit->GetGameObject()->GetTransform()->GetWorldPosition().y;
        }
        else
        {
            this->previewStartPos = unit->GetGameObject()->GetTransform()->GetWorldPosition();
        }
    }
    break;
    }
}

UnitSkillCommand::~UnitSkillCommand()
{

}

yunuGI::ITexture* UnitSkillCommand::GetIconTexture()
{
    static yunuGI::ITexture* iconRobinQ = graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/NewFolder/Skill_RobinQ.dds");
    static yunuGI::ITexture* iconRobinW = graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/NewFolder/Skill_RobinW.dds");
    static yunuGI::ITexture* iconUrsulaQ = graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/NewFolder/Skill_UrsulaQ.dds");
    static yunuGI::ITexture* iconUrsulaW = graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/NewFolder/Skill_UrsulaW.dds");
    static yunuGI::ITexture* HanselQ = graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/NewFolder/Skill_HanselQ.dds");
    static yunuGI::ITexture* HanselW = graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/NewFolder/Skill_HanselW.dds");
    switch (skillType)
    {
    case SkillType::ROBIN_Q: return iconRobinQ;
    case SkillType::ROBIN_W: return iconRobinW;
    case SkillType::URSULA_Q: return iconUrsulaQ;
    case SkillType::URSULA_W: return iconUrsulaW;
    case SkillType::HANSEL_Q: return HanselQ;
    case SkillType::HANSEL_W: return HanselW;
    }
    return nullptr;
}

void UnitSkillCommand::Execute()
{
    switch (skillType)
    {
    case SkillType::ROBIN_Q:
        unit->OrderSkill(RobinChargeSkill{  }, this->skillTargetPos);
        break;
    case SkillType::ROBIN_W:
        unit->OrderSkill(RobinTauntSkill{  }, this->expectedPos);
        break;
    case SkillType::URSULA_Q:
        unit->OrderSkill(UrsulaBlindSkill{  }, this->skillTargetPos);
        break;
    case SkillType::URSULA_W:
        unit->OrderSkill(UrsulaParalysisSkill{  }, this->skillTargetPos);
        break;
    case SkillType::HANSEL_Q:
        unit->OrderSkill(HanselChargeSkill{}, this->skillTargetPos);
        break;
    case SkillType::HANSEL_W:
        unit->OrderSkill(HanselProjectileSkill{}, this->skillTargetPos);
        break;
    }

    unit->OnStateExitCallback()[UnitBehaviourTree::SkillOnGoing].AddVolatileCallback([&]() {
        this->isDone = true;
        });
}

void UnitSkillCommand::ShowPreviewMesh()
{
    if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Robin")
    {
        if (this->skillType == SkillType::ROBIN_Q)
        {
            SkillPreviewSystem::Instance().ShowRobinQSkill(
                this->previewStartPos
            );
        }
        else if (this->skillType == SkillType::ROBIN_W)
        {
            SkillPreviewSystem::Instance().ShowRobinWSkill(this->previewStartPos,
                RobinTauntSkill::GetSkillRadius());
        }
    }
    else if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Ursula")
    {
        auto mainCam = yunutyEngine::graphics::Camera::GetMainCamera();
        auto distToXZPlane = abs(mainCam->GetTransform()->GetWorldPosition().y);
        auto projectedPoint = mainCam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), distToXZPlane, Vector3d(0, 1, 0));

        if (this->skillType == SkillType::URSULA_Q)
        {
            UrsulaBlindSkill::UpdatePosition(this->previewStartPos, projectedPoint);
            auto pos1 = UrsulaBlindSkill::GetSkillObjectPos_Left(projectedPoint);
            auto pos2 = UrsulaBlindSkill::GetSkillObjectPos_Right(projectedPoint);
            auto pos3 = UrsulaBlindSkill::GetSkillObjectPos_Top(projectedPoint);
            SkillPreviewSystem::Instance().ShowUrsulaQSkill(pos1, pos2, pos3, Vector3d::one * UrsulaBlindSkill::pod.skillRadius);
        }
        else if (this->skillType == SkillType::URSULA_W)
        {
            SkillPreviewSystem::Instance().ShowUrsulaWSkill(projectedPoint, UrsulaParalysisSkill::GetSkillRadius());
        }
    }
    else if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Hansel")
    {
        auto mainCam = yunutyEngine::graphics::Camera::GetMainCamera();
        auto distToXZPlane = abs(mainCam->GetTransform()->GetWorldPosition().y);
        auto projectedPoint = mainCam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), distToXZPlane, Vector3d(0, 1, 0));

        if (this->skillType == SkillType::HANSEL_Q)
        {
            SkillPreviewSystem::Instance().ShowHanselQSkill(projectedPoint, HanselChargeSkill::pod.skillRadius);
        }
        else if (this->skillType == SkillType::HANSEL_W)
        {
            SkillPreviewSystem::Instance().ShowHanselWSkill(this->previewStartPos);
        }
    }
}

void UnitSkillCommand::HidePreviewMesh()
{
    if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Robin")
    {
        if (this->skillType == SkillType::ROBIN_Q)
        {
            SkillPreviewSystem::Instance().HideRobinQSkill();
        }
        else if (this->skillType == SkillType::ROBIN_W)
        {
            SkillPreviewSystem::Instance().HideRobinWSkill();
        }
    }
    else if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Ursula")
    {
        if (this->skillType == SkillType::URSULA_Q)
        {
            SkillPreviewSystem::Instance().HideUrsulaQSkill();
        }
        else if (this->skillType == SkillType::URSULA_W)
        {
            SkillPreviewSystem::Instance().HideUrsulaWSkill();
        }
    }
    else if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Hansel")
    {
        if (this->skillType == SkillType::HANSEL_Q)
        {
            SkillPreviewSystem::Instance().HideHanselQSkill();
        }
        else if (this->skillType == SkillType::HANSEL_W)
        {
            SkillPreviewSystem::Instance().HideHanselWSkill();
        }
    }
}
