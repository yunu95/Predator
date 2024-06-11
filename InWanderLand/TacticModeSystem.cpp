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
}

void TacticModeSystem::EngageTacticSystem()
{
	this->isOperating = true;
	this->isCoolTime = true;
}

bool TacticModeSystem::EnqueueCommand(std::shared_ptr<UnitCommand> command)
{
	if (isOperating == false)
	{
		return false;
	}
	if (this->commandCount == this->MAX_COMMAND_COUNT)
	{
		return false;
	}

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
		return path;
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
		return path;
	}
}

void TacticModeSystem::PopCommand()
{
	this->commandCount--;
	this->commandQueue.back()->SetIsDone(true);
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
}

yunutyEngine::coroutine::Coroutine TacticModeSystem::ExecuteInternal()
{
	for (auto& each : commandQueue)
	{
		PlayerController::Instance().SelectPlayerUnit(static_cast<PlayerCharacterType::Enum>(each->GetUnit()->GetUnitTemplateData().pod.playerUnitType.enumValue));
		each->Execute();
		while (!each->IsDone())
		{
			co_await std::suspend_always();
		}
		this->commandCount--;
		each->HidePreviewMesh();
	}

	this->commandQueue.clear();
	PlayerController::Instance().SetState(PlayerController::State::Battle);

	this->isExecuting = false;
	this->isOperating = false;
}
