#include "TacticModeSystem.h"
#include "Unit.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "PlayerController.h"
#include "SkillPreviewSystem.h"
#include "PlaytimeWave.h"
#include "WaveData.h"
#include "LocalTimeEntityManager.h"
#include "PlayerSkillSystem.h"
#include "UnitCommand.h"
#include "UnitSkillCommand.h"
#include "InWanderLand.h"
#include "ITacticObject.h"

static constexpr std::array<UIEnumID, 12> commandIcons
{
    UIEnumID::TacticModeCommandIcon1,
    UIEnumID::TacticModeCommandIcon2,
    UIEnumID::TacticModeCommandIcon3,
    UIEnumID::TacticModeCommandIcon4,
    UIEnumID::TacticModeCommandIcon5,
    UIEnumID::TacticModeCommandIcon6,
    UIEnumID::TacticModeCommandIcon7,
    UIEnumID::TacticModeCommandIcon8,
    UIEnumID::TacticModeCommandIcon9,
    UIEnumID::TacticModeCommandIcon10,
    UIEnumID::TacticModeCommandIcon11,
    UIEnumID::TacticModeCommandIcon12
};

void TacticModeSystem::OnEnable()
{

}

void TacticModeSystem::Start()
{
    // 나중에는 글로벌 컨스탄트에서 데이터 받아오기
    this->coolTime = 3.f;

}

void TacticModeSystem::Update()
{
    if (isOperating == false && this->isCoolTime == true)
    {
        this->elapsedTime += Time::GetDeltaTime();

        if (this->elapsedTime < this->coolTime)
        {
            this->isCoolTime = true;
        }
        else
        {
            this->isCoolTime = false;
            this->elapsedTime = 0.f;
        }

        if (isCoolTime)
        {
            UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Overlay)->adjuster->SetTargetFloat(this->elapsedTime / this->coolTime);
            UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->EnableElement();
            UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->SetNumber(this->coolTime - this->elapsedTime);
        }
        else
        {
            UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Overlay)->adjuster->SetTargetFloat(1);
            UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->DisableElement();
        }
    }
    else
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Overlay)->adjuster->SetTargetFloat(1);
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->DisableElement();
    }
}

void TacticModeSystem::OnContentsStop()
{
    SkillPreviewSystem::Instance().HideTemporaryRoute();
    this->ClearCommand();
    GetComponent()->SetActive(false);
    // 플레이가 중단되면 모든 Pause는 되돌려준다.

    for (auto& each : playersPauseRevArr)
    {
        each.reset();
    }
    this->isExecuting = false;
    this->isOperating = false;
}

void TacticModeSystem::EngageTacticSystem()
{
    this->isOperating = true;
    this->isCoolTime = true;

    playersPauseRevArr[0] = PlayerController::Instance().GetPlayers()[0].lock()->referencePause.Acquire();
    playersPauseRevArr[1] = PlayerController::Instance().GetPlayers()[1].lock()->referencePause.Acquire();
    playersPauseRevArr[2] = PlayerController::Instance().GetPlayers()[2].lock()->referencePause.Acquire();

    auto wave = PlaytimeWave::GetCurrentOperatingWave();
    if (!wave.expired())
    {
        wave.lock()->StopWaveElapsedTime();
    }
  
    SFXManager::PlaySoundfile("sounds/Tactical mode/Tactical mode on.wav");
    ITacticObject::OnPauseAll();
    SyncWithTacticCommandQueueUI();
}

EnqueErrorType TacticModeSystem::EnqueueCommand(std::shared_ptr<UnitCommand> command)
{
    EnqueErrorType errorType = EnqueErrorType::NONE;
    // 큐가 가득 차 있는지 검사하는 코드
    if (this->commandList.size() == this->MAX_COMMAND_COUNT)
    {
        errorType = EnqueErrorType::QueueFull;
        return errorType;
    }

    // 들어 온 명령을 수행 할 마나가 있는지에 대한 여부
    auto mana = PlayerController::Instance().GetMana();
    if (mana < command->GetCommandCost())
    {
        errorType = EnqueErrorType::NotEnoughMana;
        return errorType;
    }

    if (command->GetCommandType() == UnitCommand::CommandType::Skill)
    {
        if (this->useSkill[std::static_pointer_cast<UnitSkillCommand>(command)->GetSkillType()] == true)
        {
            errorType = EnqueErrorType::UseSkill;
            return errorType;
        }
        this->useSkill[std::static_pointer_cast<UnitSkillCommand>(command)->GetSkillType()] = true;
    }

    PlayerController::Instance().SetMana(mana - command->GetCommandCost());

    this->commandList.push_back(command);

    command->ShowPreviewMesh();

    // 유닛의 타입에 따라 멤버변수 3개에 알맞게 넣어줘야한다.
    if (command->GetUnit()->GetUnitTemplateData().GetDataResourceName() == "SKM_Robin")
    {
        this->robinLastCommand = command;
    }
    else if (command->GetUnit()->GetUnitTemplateData().GetDataResourceName() == "SKM_Ursula")
    {
        this->ursulaLastCommand = command;
    }
    else if (command->GetUnit()->GetUnitTemplateData().GetDataResourceName() == "SKM_Hansel")
    {
        this->hanselLastCommand = command;
    }

    errorType = EnqueErrorType::Success;
    SFXManager::PlaySoundfile("sounds/Tactical mode/Tactical mode skill registration.wav");
    SyncWithTacticCommandQueueUI();
    return errorType;
}

void TacticModeSystem::ExecuteCommands()
{
    SkillPreviewSystem::Instance().HideTemporaryRoute();
    this->isExecuting = true;
    StartCoroutine(this->ExecuteInternal());
}

void TacticModeSystem::ShowTemporaryRouteInTacticMode(PlayerCharacterType::Enum playerType)
{
    if (this->isExecuting)
    {
        return;
    }
    std::vector<Vector3d> path;
    auto characters = PlayerController::Instance().GetPlayers();
    auto mainCam = yunutyEngine::graphics::Camera::GetMainCamera();
    auto distToXZPlane = abs(mainCam->GetTransform()->GetWorldPosition().y);
    auto projectedPoint = mainCam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), distToXZPlane, Vector3d(0, 1, 0));
    switch (playerType)
    {
    case PlayerCharacterType::Robin:
    {
        if (this->robinLastCommand)
        {
            path = SingleNavigationField::Instance().GetSmoothPath(this->robinLastCommand->GetExpectedPos(), projectedPoint);
        }
        else
        {
            path = SingleNavigationField::Instance().GetSmoothPath(characters[playerType].lock()->GetGameObject()->GetTransform()->GetWorldPosition(), projectedPoint);
        }
        SkillPreviewSystem::Instance().ShowTemporaryRoute(SkillPreviewSystem::UnitType::Robin, path);
    }
    break;
    case PlayerCharacterType::Ursula:
    {
        if (this->ursulaLastCommand)
        {
            path = SingleNavigationField::Instance().GetSmoothPath(this->ursulaLastCommand->GetExpectedPos(), projectedPoint);
        }
        else
        {
            path = SingleNavigationField::Instance().GetSmoothPath(characters[playerType].lock()->GetGameObject()->GetTransform()->GetWorldPosition(), projectedPoint);
        }
        SkillPreviewSystem::Instance().ShowTemporaryRoute(SkillPreviewSystem::UnitType::Ursula, path);
    }
    break;
    case PlayerCharacterType::Hansel:
    {
        if (this->hanselLastCommand)
        {
            path = SingleNavigationField::Instance().GetSmoothPath(this->hanselLastCommand->GetExpectedPos(), projectedPoint);
        }
        else
        {
            path = SingleNavigationField::Instance().GetSmoothPath(characters[playerType].lock()->GetGameObject()->GetTransform()->GetWorldPosition(), projectedPoint);
        }
        SkillPreviewSystem::Instance().ShowTemporaryRoute(SkillPreviewSystem::UnitType::Hansel, path);
    }
    break;
    }
}

void TacticModeSystem::ShowSkillPreviewInTacticMode(SkillType::Enum skillType)
{
    auto characters = PlayerController::Instance().GetPlayers();

    auto mainCam = yunutyEngine::graphics::Camera::GetMainCamera();
    auto distToXZPlane = abs(mainCam->GetTransform()->GetWorldPosition().y);
    auto projectedPoint = mainCam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), distToXZPlane, Vector3d(0, 1, 0));

    switch (skillType)
    {
    case SkillType::ROBIN_Q:
    {
        if (useSkill[skillType] == false)
        {
            if (this->robinLastCommand)
            {
                SkillPreviewSystem::Instance().ShowRobinQSkill(this->robinLastCommand->GetExpectedPos());
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Robin, this->robinLastCommand->GetExpectedPos(), RobinChargeSkill::GetMaxDistance());
            }
            else
            {
                SkillPreviewSystem::Instance().ShowRobinQSkill(characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition());
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Robin, characters[PlayerCharacterType::Robin].lock()->GetTransform()->GetWorldPosition(), RobinChargeSkill::GetMaxDistance());
            }
        }
    }
    break;
    case SkillType::URSULA_Q:
    {
        if (useSkill[skillType] == false)
        {
            if (this->ursulaLastCommand)
            {
                UrsulaBlindSkill::UpdatePosition(this->ursulaLastCommand->GetExpectedPos(), projectedPoint);
                auto pos1 = UrsulaBlindSkill::GetSkillObjectPos_Left(projectedPoint);
                auto pos2 = UrsulaBlindSkill::GetSkillObjectPos_Right(projectedPoint);
                auto pos3 = UrsulaBlindSkill::GetSkillObjectPos_Top(projectedPoint);
                SkillPreviewSystem::Instance().ShowUrsulaQSkill(pos1, pos2, pos3, Vector3d::one * UrsulaBlindSkill::pod.skillRadius);
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Ursula, this->ursulaLastCommand->GetExpectedPos(), UrsulaBlindSkill::GetSkillRange());
            }
            else
            {
                UrsulaBlindSkill::UpdatePosition(characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition(), projectedPoint);
                auto pos1 = UrsulaBlindSkill::GetSkillObjectPos_Left(projectedPoint);
                auto pos2 = UrsulaBlindSkill::GetSkillObjectPos_Right(projectedPoint);
                auto pos3 = UrsulaBlindSkill::GetSkillObjectPos_Top(projectedPoint);
                SkillPreviewSystem::Instance().ShowUrsulaQSkill(pos1, pos2, pos3, Vector3d::one * UrsulaBlindSkill::pod.skillRadius);
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Ursula, characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition(), UrsulaBlindSkill::GetSkillRange());
            }
        }
    }
    break;
    case SkillType::URSULA_W:
    {
        if (useSkill[skillType] == false)
        {
            if (this->ursulaLastCommand)
            {
                SkillPreviewSystem::Instance().ShowUrsulaWSkill(projectedPoint, UrsulaParalysisSkill::GetSkillRadius());
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Ursula, this->ursulaLastCommand->GetExpectedPos(), UrsulaParalysisSkill::pod.skillRange);
            }
            else
            {
                SkillPreviewSystem::Instance().ShowUrsulaWSkill(projectedPoint, UrsulaParalysisSkill::GetSkillRadius());
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Ursula, characters[PlayerCharacterType::Ursula].lock()->GetTransform()->GetWorldPosition(), UrsulaParalysisSkill::pod.skillRange);
            }
        }
    }
    break;
    case SkillType::HANSEL_Q:
    {
        if (useSkill[skillType] == false)
        {
            if (this->hanselLastCommand)
            {
                SkillPreviewSystem::Instance().ShowHanselQSkill(projectedPoint, HanselChargeSkill::pod.skillRadius);
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Hansel, this->hanselLastCommand->GetExpectedPos(), HanselChargeSkill::GetMaxRange());
            }
            else
            {
                SkillPreviewSystem::Instance().ShowHanselQSkill(projectedPoint, HanselChargeSkill::pod.skillRadius);
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Hansel, characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition(), HanselChargeSkill::GetMaxRange());
            }
        }
    }
    break;
    case SkillType::HANSEL_W:
    {
        if (useSkill[skillType] == false)
        {
            if (this->hanselLastCommand)
            {
                SkillPreviewSystem::Instance().ShowHanselWSkill(this->hanselLastCommand->GetExpectedPos());
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Hansel, this->hanselLastCommand->GetExpectedPos(), HanselProjectileSkill::GetMaxRange());
            }
            else
            {
                SkillPreviewSystem::Instance().ShowHanselWSkill(characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition());
                SkillPreviewSystem::Instance().ShowSkillMaxRange(SkillPreviewSystem::UnitType::Hansel, characters[PlayerCharacterType::Hansel].lock()->GetTransform()->GetWorldPosition(), HanselProjectileSkill::GetMaxRange());
            }
        }
    }
    break;
    }
}

std::vector<Vector3d> TacticModeSystem::GetPathInTacticMode(PlayerCharacterType::Enum playerType, Unit* unit)
{
    std::vector<Vector3d> path;
    auto characters = PlayerController::Instance().GetPlayers();
    auto mainCam = yunutyEngine::graphics::Camera::GetMainCamera();
    if (unit == nullptr)
    {
        auto distToXZPlane = abs(mainCam->GetTransform()->GetWorldPosition().y);
        auto projectedPoint = mainCam->GetProjectedPoint(Input::getMouseScreenPositionNormalizedZeroCenter(), distToXZPlane, Vector3d(0, 1, 0));
        switch (playerType)
        {
        case PlayerCharacterType::Robin:
        {
            if (this->robinLastCommand)
            {
                path = SingleNavigationField::Instance().GetSmoothPath(this->robinLastCommand->GetExpectedPos(), projectedPoint);
            }
            else
            {
                path = SingleNavigationField::Instance().GetSmoothPath(characters[playerType].lock()->GetGameObject()->GetTransform()->GetWorldPosition(), projectedPoint);
            }
        }
        break;
        case PlayerCharacterType::Ursula:
        {
            if (this->ursulaLastCommand)
            {
                path = SingleNavigationField::Instance().GetSmoothPath(this->ursulaLastCommand->GetExpectedPos(), projectedPoint);
            }
            else
            {
                path = SingleNavigationField::Instance().GetSmoothPath(characters[playerType].lock()->GetGameObject()->GetTransform()->GetWorldPosition(), projectedPoint);
            }
        }
        break;
        case PlayerCharacterType::Hansel:
        {
            if (this->hanselLastCommand)
            {
                path = SingleNavigationField::Instance().GetSmoothPath(this->hanselLastCommand->GetExpectedPos(), projectedPoint);
            }
            else
            {
                path = SingleNavigationField::Instance().GetSmoothPath(characters[playerType].lock()->GetGameObject()->GetTransform()->GetWorldPosition(), projectedPoint);
            }
        }
        break;
        }
    }
    else
    {
        switch (playerType)
        {
        case PlayerCharacterType::Robin:
        {
            if (this->robinLastCommand)
            {
                path = SingleNavigationField::Instance().GetSmoothPath(this->robinLastCommand->GetExpectedPos(), unit->GetGameObject()->GetTransform()->GetWorldPosition());
            }
            else
            {
                path = SingleNavigationField::Instance().GetSmoothPath(characters[playerType].lock()->GetGameObject()->GetTransform()->GetWorldPosition(), unit->GetGameObject()->GetTransform()->GetWorldPosition());
            }
        }
        break;
        case PlayerCharacterType::Ursula:
        {
            if (this->ursulaLastCommand)
            {
                path = SingleNavigationField::Instance().GetSmoothPath(this->ursulaLastCommand->GetExpectedPos(), unit->GetGameObject()->GetTransform()->GetWorldPosition());
            }
            else
            {
                path = SingleNavigationField::Instance().GetSmoothPath(characters[playerType].lock()->GetGameObject()->GetTransform()->GetWorldPosition(), unit->GetGameObject()->GetTransform()->GetWorldPosition());
            }
        }
        break;
        case PlayerCharacterType::Hansel:
        {
            if (this->hanselLastCommand)
            {
                path = SingleNavigationField::Instance().GetSmoothPath(this->hanselLastCommand->GetExpectedPos(), unit->GetGameObject()->GetTransform()->GetWorldPosition());
            }
            else
            {
                path = SingleNavigationField::Instance().GetSmoothPath(characters[playerType].lock()->GetGameObject()->GetTransform()->GetWorldPosition(), unit->GetGameObject()->GetTransform()->GetWorldPosition());
            }
        }
        break;
        }
    }

    for (auto& each : path)
    {
        each.y = 0.1f;
    }
    return path;
}

bool TacticModeSystem::CanSelectSkill(SkillType::Enum skillType)
{
    return this->useSkill[skillType] == false;
}

void TacticModeSystem::PopCommand()
{
    if (this->commandList.empty()) return;

    auto command = this->commandList.back();
    if (command->GetCommandType() == UnitCommand::Skill)
    {
        this->useSkill[std::static_pointer_cast<UnitSkillCommand>(command)->GetSkillType()] = false;
    }
    command->HidePreviewMesh();
    PlayerController::Instance().SetMana(PlayerController::Instance().GetMana() + command->GetCommandCost());
    this->commandList.pop_back();

    bool robinCommand = false;
    bool ursulaCommand = false;
    bool hanselCommand = false;

    for (auto it = commandList.rbegin(); it != commandList.rend(); ++it)
    {
        auto commandUnitType = static_cast<PlayerCharacterType::Enum>(command->GetUnit()->GetUnitTemplateData().pod.playerUnitType.enumValue);
        auto command = *it;
        if ((commandUnitType == static_cast<PlayerCharacterType::Enum>(command->GetUnit()->GetUnitTemplateData().pod.playerUnitType.enumValue)))
        {
            switch (commandUnitType)
            {
            case PlayerCharacterType::Robin:
            {
                robinCommand = true;
                this->robinLastCommand = command;
            }
            break;
            case PlayerCharacterType::Ursula:
            {
                ursulaCommand = true;
                this->ursulaLastCommand = command;
            }
            break;
            case PlayerCharacterType::Hansel:
            {
                hanselCommand = true;
                this->hanselLastCommand = command;
            }
            break;
            }

            break;
        }
    }

    if (!robinCommand)
    {
        this->robinLastCommand = nullptr;
    }
    if (!ursulaCommand)
    {
        this->ursulaLastCommand = nullptr;
    }
    if (!hanselCommand)
    {
        this->hanselLastCommand = nullptr;
    }
    SyncWithTacticCommandQueueUI();
}

void TacticModeSystem::InterruptedCommand(Unit* unit)
{
    auto unitType = static_cast<PlayerCharacterType::Enum>(unit->GetUnitTemplateData().pod.playerUnitType.enumValue);

    for (auto iter = this->commandList.begin(); iter != this->commandList.end(); ++iter)
    {
        if (static_cast<PlayerCharacterType::Enum>((*iter)->GetUnit()->GetUnitTemplateData().pod.playerUnitType.enumValue) == unitType)
        {
            (*iter)->SetIsDone(true);
            (*iter)->HidePreviewMesh();
            if ((*iter)->GetCommandType() == UnitCommand::CommandType::Skill)
            {
                useSkill[std::static_pointer_cast<UnitSkillCommand>((*iter))->GetSkillType()] = false;
            }
        }
    }

    switch (unitType)
    {
    case PlayerCharacterType::Robin:
    {
        this->robinLastCommand = nullptr;
    }
    break;
    case PlayerCharacterType::Ursula:
    {
        this->ursulaLastCommand = nullptr;
    }
    break;
    case PlayerCharacterType::Hansel:
    {
        this->hanselLastCommand = nullptr;
    }
    break;
    }
}

void TacticModeSystem::ClearCommand()
{
    for (auto& each : this->commandList)
    {
        each->HidePreviewMesh();
        PlayerController::Instance().SetMana(PlayerController::Instance().GetMana() + each->GetCommandCost());
    }

    this->commandList.clear();
    for (auto& each : this->useSkill)
    {
        each = false;
    }
    this->robinLastCommand = nullptr;
    this->ursulaLastCommand = nullptr;
    this->hanselLastCommand = nullptr;
    SyncWithTacticCommandQueueUI();
}

yunutyEngine::coroutine::Coroutine TacticModeSystem::ExecuteInternal()
{
    int iconIndex = 0;
    for (auto& each : this->commandList)
    {
        if (!each->IsDone())
        {
            PlayerController::Instance().SelectPlayerUnit(static_cast<PlayerCharacterType::Enum>(each->GetUnit()->GetUnitTemplateData().pod.playerUnitType.enumValue));
            // 현재 명령을 수행하는 플레이어 유닛은 움직인다.
            this->playersPauseRevArr[each->GetPlayerType()].reset();
            each->Execute();
            while (!each->IsDone())
            {
                co_await std::suspend_always();
            }
            // 명령 수행이 끝나면 다시 멈춘다.
            this->playersPauseRevArr[each->GetPlayerType()] = PlayerController::Instance().GetPlayers()[each->GetPlayerType()].lock()->referencePause.Acquire();
            each->HidePreviewMesh();
        }
        UIManager::Instance().GetUIElementByEnum(commandIcons[iconIndex])->DisableElement();
        iconIndex++;
    }

    this->commandList.clear();
    PlayerController::Instance().SetState(PlayerController::State::Battle);

    this->isExecuting = false;
    this->isOperating = false;
    this->robinLastCommand = nullptr;
    this->ursulaLastCommand = nullptr;
    this->hanselLastCommand = nullptr;

    // 전술모드 명령이 끝나면 플레이어들은 다시 움직인다.
    for (auto& each : playersPauseRevArr)
    {
        each.reset();
    }
    for (auto& each : useSkill)
    {
        each = false;
    }

    // Wave의 시간도 흐른다.
    auto wave = PlaytimeWave::GetCurrentOperatingWave();
    if (!wave.expired())
    {
        wave.lock()->ResumeWaveElapsedTime();
    }

    SFXManager::PlaySoundfile("sounds/Tactical mode/Tactical mode off.wav");
    ITacticObject::OnResumeAll();
}

void TacticModeSystem::SyncWithTacticCommandQueueUI()
{
    int idx = 0;
    for (auto& eachCmd : commandList)
    {
        eachCmd->GetCommandType();
        auto uiElement = UIManager::Instance().GetUIElementByEnum(commandIcons[idx]);
        uiElement->EnableElement();
        uiElement->imageComponent.lock()->GetGI().SetImage(eachCmd->GetIconTexture());
        idx++;
    }
    while (idx < commandIcons.size())
    {
        UIManager::Instance().GetUIElementByEnum(commandIcons[idx])->DisableElement();
        idx++;
    }
}
