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

	switch (skillType)
	{
		case SkillType::ROBIN_Q:  this->commandCost = RobinChargeSkill::pod.cost;
			break;
		case SkillType::ROBIN_W: this->commandCost = RobinTauntSkill::pod.skillCost;
			break;
		case SkillType::URSULA_Q: this->commandCost = UrsulaBlindSkill::pod.skillCost;
			break;
		case SkillType::URSULA_W: this->commandCost = UrsulaParalysisSkill::pod.skillCost;
			break;
		case SkillType::HANSEL_Q: this->commandCost = HanselChargeSkill::pod.skillCost;
			break;
		case SkillType::HANSEL_W: this->commandCost = HanselProjectileSkill::pod.skillCost;
			break;
	}
}

UnitSkillCommand::~UnitSkillCommand()
{

}

void UnitSkillCommand::Execute()
{

}

void UnitSkillCommand::ShowPreviewMesh()
{
	//if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Robin")
	//{
	//	if (this->skillType.ROBIN_Q == SkillType::ROBIN_Q)
	//	{
	//		SkillPreviewSystem::Instance().ShowRobinQSkill(
	//			TacticModeSystem::Instance().GetRobinLastCommand()->GetExpectedPos()
	//		);
	//	}
	//	else if (this->skillType.ROBIN_W == SkillType::ROBIN_W)
	//	{
	//		SkillPreviewSystem::Instance().ShowRobinWSkill(TacticModeSystem::Instance().GetRobinLastCommand()->GetExpectedPos(),
	//			RobinTauntSkill::pod.skillRadius);
	//	}
	//}
	//else if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Ursula")
	//{
	//	auto mainCam = yunutyEngine::graphics::Camera::GetMainCamera();
	//	auto distToXZPlane = abs(mainCam->GetTransform()->GetWorldPosition().y);
	//	auto projectedPoint = mainCam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), distToXZPlane, Vector3d(0, 1, 0));

	//	if (this->skillType.URSULA_Q == SkillType::URSULA_Q)
	//	{
	//		UrsulaBlindSkill::UpdatePosition(TacticModeSystem::Instance().GetUrsulaLastCommand()->GetExpectedPos(), projectedPoint);
	//		auto pos1 = UrsulaBlindSkill::GetSkillObjectPos_Left(projectedPoint);
	//		auto pos2 = UrsulaBlindSkill::GetSkillObjectPos_Right(projectedPoint);
	//		auto pos3 = UrsulaBlindSkill::GetSkillObjectPos_Top(projectedPoint);
	//		SkillPreviewSystem::Instance().ShowUrsulaQSkill(pos1, pos2, pos3, Vector3d::one * UrsulaBlindSkill::pod.skillRadius);
	//	}
	//	else if (this->skillType.URSULA_W == SkillType::URSULA_W)
	//	{
	//		SkillPreviewSystem::Instance().ShowUrsulaWSkill(projectedPoint, UrsulaParalysisSkill::pod.skillRadius);
	//	}
	//}
	//else if (unit->GetUnitTemplateData().GetDataResourceName() == "SKM_Hansel")
	//{
	//	auto mainCam = yunutyEngine::graphics::Camera::GetMainCamera();
	//	auto distToXZPlane = abs(mainCam->GetTransform()->GetWorldPosition().y);
	//	auto projectedPoint = mainCam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), distToXZPlane, Vector3d(0, 1, 0));

	//	if (this->skillType.HANSEL_Q == SkillType::HANSEL_Q)
	//	{
	//		SkillPreviewSystem::Instance().ShowHanselQSkill(projectedPoint, HanselChargeSkill::pod.stompRadius);
	//	}
	//	else if (this->skillType.HANSEL_W == SkillType::HANSEL_W)
	//	{
	//		SkillPreviewSystem::Instance().ShowHanselWSkill(TacticModeSystem::Instance().GetHanselLastCommand()->GetExpectedPos());
	//	}
	//}
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
