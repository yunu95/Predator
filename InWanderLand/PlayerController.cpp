#include "InWanderLand.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "Unit.h"
#include "Dotween.h"
#include "SkillPreviewSystem.h"
#include "GameManager.h"
#include "CursorDetector.h"

void PlayerController::Start()
{
    currentSelectedSerialNumber = Unit::UnitType::Warrior;
}

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

void PlayerController::PlayFunction()
{
    this->SetActive(true);
    if (isOncePaused)
    {
        Start();
    }
}

void PlayerController::StopFunction()
{
    SetActive(false);
    playerComponentMap.clear();
    m_movingSystemComponent = nullptr;
    m_dotween = nullptr;
    m_cursorDetector = nullptr;
    currentSelectedSerialNumber = Unit::UnitType::Warrior;
    previousSerialNumber = 0;
    lookRotationDuration = 0.1f;
}

void PlayerController::SetRightClickFunction()
{
    if (GameManager::Instance().IsBattleSystemOperating())
    {
        if (static_cast<int>(currentSelectedSerialNumber) == InputManager::SelectedSerialNumber::All)
        {
            m_movingSystemComponent->groundRightClickCallback = [=](Vector3d pos)
                {
                    //if (!InputManager::Instance().GetInputManagerActive() || UIManager::Instance().IsMouseOnButton())
                    //{
                    //    return;
                    //}
                    if (m_cursorDetector->GetCurrentOnMouseUnit() == nullptr)
                    {
                        for (auto e : playerComponentMap)
                        {
                            e.second->OrderMove(pos);
                        }
                    }
                    else if (Unit* selectedUnit = m_cursorDetector->GetCurrentOnMouseUnit();
                        selectedUnit->GetUnitSide() == Unit::UnitSide::Enemy)
                    {
                        for (auto e : playerComponentMap)
                        {
                            e.second->OrderAttackMove(pos, selectedUnit);
                        }
                    }
                    UIManager::Instance().SummonMoveToFeedback(pos);
                };
        }
        else
        {
            Unit* currentSelectedUnit = playerComponentMap.find(currentSelectedSerialNumber)->second;
            m_movingSystemComponent->groundRightClickCallback = [=](Vector3d pos)
                {
                    //if (!InputManager::Instance().GetInputManagerActive()||UIManager::Instance().IsMouseOnButton())
                    //{
                    //    return;
                    //}
                    if (m_cursorDetector->GetCurrentOnMouseUnit() == nullptr)
                    {
                        currentSelectedUnit->OrderMove(pos);
                    }
                    else if (m_cursorDetector->GetCurrentOnMouseUnit()->GetUnitSide() == Unit::UnitSide::Enemy)
                    {
                        currentSelectedUnit->OrderAttackMove(pos, m_cursorDetector->GetCurrentOnMouseUnit());
                    }
                    UIManager::Instance().SummonMoveToFeedback(pos);
                };
        }
    }
    else
    {
        if (!playerComponentMap.contains(currentSelectedSerialNumber))
        {
            return;
        }
        Unit* currentSelectedUnit = playerComponentMap.find(currentSelectedSerialNumber)->second;
        m_movingSystemComponent->groundRightClickCallback = [=](Vector3d pos)
            {
                if (!InputManager::Instance().GetInputManagerActive() || UIManager::Instance().IsMouseOnButton())
                {
                    return;
                }
                /*for (auto e : playerComponentMap)
                {
                    e.second->OrderMove(pos);
                }*/
                currentSelectedUnit->OrderMove(pos);
                UIManager::Instance().SummonMoveToFeedback(pos);
            };
    }
}

void PlayerController::SetLeftClickAttackMove()
{
    m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
        {
            if (!InputManager::Instance().GetInputManagerActive() || UIManager::Instance().IsMouseOnButton())
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
                    if (!InputManager::Instance().GetInputManagerActive() || UIManager::Instance().IsMouseOnButton())
                    {
                        return;
                    }
                    SkillPreviewSystem::Instance().ActivateSkillPreview(false);
                    if (auto playerComp = playerComponentMap.find(currentSelectedSerialNumber); playerComp != playerComponentMap.end())
                    {
                        playerComp->second->OrderSkill(p_skillNum, pos);
                    }
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
    if (playerComponentMap.find(p_num) != playerComponentMap.end()
        || p_num == Unit::UnitType::AllPlayers)
    {
        currentSelectedSerialNumber = p_num;

        /// 비전투 상황일 때는 우클릭 함수 정의 이전에 Leader Unit으로 전환해주는 로직이 필요합니다.
        if (!GameManager::Instance().IsBattleSystemOperating())
            ChangeLeaderPlayerUnit(p_num);

		SetRightClickFunction();
    }
}

void PlayerController::ReportBattleEnded()
{
    Unit::UnitType tempLeaderType;

	for (auto e : playerComponentMap)
	{
        if (e.second->GetActive())
        {
            tempLeaderType = e.first;
            break;
        }
	}

    ChangeLeaderPlayerUnit(tempLeaderType);
}

void PlayerController::ChangeLeaderPlayerUnit(Unit::UnitType p_num)
{
    for (auto e : playerComponentMap)
    {
        e.second->ReportLeaderUnitChanged(p_num);
    }
}

std::unordered_map<Unit::UnitType, Unit*> PlayerController::GetPlayerMap() const
{
    return playerComponentMap;
}

Unit* PlayerController::FindSelectedUnitByUnitType(Unit::UnitType p_type)
{
    if (p_type != Unit::UnitType::AllPlayers)
        return playerComponentMap.find(p_type)->second;
}

Unit* PlayerController::GetCurrentSelectedPlayerUnit() const 
{
    return playerComponentMap.find(currentSelectedSerialNumber)->second;
}



