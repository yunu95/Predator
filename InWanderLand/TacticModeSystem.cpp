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
#include "InWanderLand.h"

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
	activateWaveEnemyUnitPauseRefVec.clear();
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
	wave.lock()->StopWaveElapsedTime();
	for (auto& each : wave.lock()->m_currentWaveUnitVector)
	{
		activateWaveEnemyUnitPauseRefVec.push_back(each->referencePause.Acquire());
	}
}

EnqueErrorType TacticModeSystem::EnqueueCommand(std::shared_ptr<UnitCommand> command)
{
	EnqueErrorType errorType = EnqueErrorType::NONE;

	// 큐가 가득 차 있는지 검사하는 코드
	if (this->commandCount == this->MAX_COMMAND_COUNT)
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

	PlayerController::Instance().SetMana(mana - command->GetCommandCost());
	this->commandCount++;

	this->commandQueue.push_back(command);

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

void TacticModeSystem::PopCommand()
{
	this->commandCount--;
	this->commandQueue.back()->SetIsDone(true);
	auto backCommandUnitType = static_cast<PlayerCharacterType::Enum>(this->commandQueue.back()->GetUnit()->GetUnitTemplateData().pod.playerUnitType.enumValue);

	for (auto it = commandQueue.rbegin(); it != commandQueue.rend(); ++it)
	{
		auto command = *it;
		if ((!command->IsDone()) &&
			(backCommandUnitType == static_cast<PlayerCharacterType::Enum>(command->GetUnit()->GetUnitTemplateData().pod.playerUnitType.enumValue)))
		{
			switch (backCommandUnitType)
			{
				case PlayerCharacterType::Robin:
				{
					this->robinLastCommand = command;
				}
				break;
				case PlayerCharacterType::Ursula:
				{
					this->ursulaLastCommand = command;
				}
				break;
				case PlayerCharacterType::Hansel:
				{
					this->hanselLastCommand = command;
				}
				break;
			}

			break;
		}
	}
}

void TacticModeSystem::InterruptedCommand(Unit* unit)
{
	auto unitType = static_cast<PlayerCharacterType::Enum>(unit->GetUnitTemplateData().pod.playerUnitType.enumValue);

	for (auto& each : commandQueue)
	{
		if (static_cast<PlayerCharacterType::Enum>(each->GetUnit()->GetUnitTemplateData().pod.playerUnitType.enumValue) == unitType)
		{
			each->SetIsDone(true);
			this->commandCount--;
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
	for (auto& each : this->commandQueue)
	{
		each->HidePreviewMesh();
	}

	this->commandCount = 0;
	this->commandQueue.clear();
	PlayerController::Instance().SetState(PlayerController::State::None);
	this->robinLastCommand = nullptr;
	this->ursulaLastCommand = nullptr;
	this->hanselLastCommand = nullptr;
}

yunutyEngine::coroutine::Coroutine TacticModeSystem::ExecuteInternal()
{
	for (auto& each : commandQueue)
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
		this->playersPauseRevArr[each->GetPlayerType()] = PlayerController::Instance().GetPlayers()[each->GetPlayerType()].lock()->referencePause.Acquire();;
		this->commandCount--;
		each->HidePreviewMesh();
	}

	this->commandQueue.clear();
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

	// Wave의 시간도 흐른다.
	auto wave = PlaytimeWave::GetCurrentOperatingWave();
	wave.lock()->ResumeWaveElapsedTime();
	// Wave의 적 유닛들도 움직인다.
	activateWaveEnemyUnitPauseRefVec.clear();
}
