#include "InWanderLand.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "Unit.h"
#include "Dotween.h"
#include "SkillPreviewSystem.h"
#include "GameManager.h"

void PlayerController::SetMovingSystemComponent(RTSCam* sys)
{
    m_movingSystemComponent = sys;
    m_dotween = sys->GetGameObject()->GetComponent<Dotween>();
}

void PlayerController::AddPlayerUnit(Unit* p_playerUnit)
{
    playerComponentMap.insert({ p_playerUnit->GetPlayerSerialNumber(), p_playerUnit });
}

void PlayerController::ErasePlayerUnit(Unit* p_playerUnit)
{
	playerComponentMap.erase(p_playerUnit->GetUnitType());
}

void PlayerController::SetLeftClickMove()
{
    if (GameManager::Instance().IsBattleSystemOperating())
    {
        if (static_cast<int>(currentSelectedSerialNumber) == InputManager::SelectedSerialNumber::All)
        {
            m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
                {
                    if (UIManager::Instance().IsMouseOnButton())
                    {
                        return;
                    }
                    for (auto e : playerComponentMap)
                    {
                        e.second->OrderMove(pos);
                    }
                };
        }
        else
        {
            Unit* currentSelectedUnit = playerComponentMap.find(currentSelectedSerialNumber)->second;
            m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
                {
                    if (UIManager::Instance().IsMouseOnButton())
                    {
                        return;
                    }
                    currentSelectedUnit->OrderMove(pos);
                };
        }
    }
    else
    {
        /// 세 플레이어 유닛이 offset을 갖고 이동할 수 있도록 하기
        m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
            {
                if (UIManager::Instance().IsMouseOnButton())
                {
                    return;
                }
                for (auto e : playerComponentMap)
                {
                    e.second->OrderMove(pos);
                }
            };
    }
}

void PlayerController::SetLeftClickAttackMove()
{
    m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
        {
            if (UIManager::Instance().IsMouseOnButton())
            {
                return;
            }
            playerComponentMap.find(currentSelectedSerialNumber)->second->OrderAttackMove(pos);
        };
}

void PlayerController::SetLeftClickSkill(Unit::SkillEnum p_skillNum)
{
    Unit* currentUnit = playerComponentMap.find(currentSelectedSerialNumber)->second;

    if (currentUnit->GetCurrentUnitState() != Unit::UnitState::Skill)
    {
        if (static_cast<int>(currentSelectedSerialNumber) == InputManager::SelectedSerialNumber::One && p_skillNum == Unit::SkillEnum::W)
        {
            /// Warrior의 W 스킬은 마우스로 클릭하지 않아도 바로 실행되는 스킬이다. 다른 스킬 나온다면 구조적 개선 필요
            playerComponentMap.find(currentSelectedSerialNumber)->second->OrderSkill(p_skillNum);
        }
        else
        {
            Unit* currentSelectedUnit = playerComponentMap.find(currentSelectedSerialNumber)->second;
            SkillPreviewSystem::Instance().SetCurrentSelectedPlayerUnit(currentSelectedUnit);
            SkillPreviewSystem::Instance().SetCurrentSkillPreviewType(currentSelectedUnit->GetSkillPreviewType(p_skillNum));
            SkillPreviewSystem::Instance().SetCurrentSelectedSkillNum(p_skillNum);
            SkillPreviewSystem::Instance().ActivateSkillPreview(true);
            m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
                {
                    if (UIManager::Instance().IsMouseOnButton())
                    {
                        return;
                    }
                    SkillPreviewSystem::Instance().ActivateSkillPreview(false);
                    playerComponentMap.find(currentSelectedSerialNumber)->second->OrderSkill(p_skillNum, pos);
                };
        }
    }
}

void PlayerController::SetLeftClickEmpty()
{
    m_movingSystemComponent->groundLeftClickCallback = [](Vector3d pos) {};
}


void PlayerController::SetRightClickEmpty()
{
    m_movingSystemComponent->groundRightClickCallback = [](Vector3d pos) {};
}

void PlayerController::SetCurrentPlayerSerialNumber(Unit::UnitType p_num)
{
    if (playerComponentMap.find(p_num) != playerComponentMap.end() || p_num == Unit::UnitType::AllPlayers)
    {
        currentSelectedSerialNumber = p_num;
        SetLeftClickMove();
    }
}

std::unordered_map<Unit::UnitType, Unit*> PlayerController::GetPlayerMap() const
{
    return playerComponentMap;
}

Unit* PlayerController::FindSelectedUnitByUnitType(Unit::UnitType p_type)
{
    return playerComponentMap.find(p_type)->second;
}



