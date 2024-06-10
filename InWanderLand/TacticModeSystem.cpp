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

void TacticModeSystem::OperateTacticSystem()
{
	this->isOperating = true;
	this->isCoolTime = true;
}

void TacticModeSystem::Enqueue(std::shared_ptr<UnitCommand> command)
{
	if (isOperating == false) return;

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

void TacticModeSystem::Execute()
{
	this->isExecuting = true;
	StartCoroutine(this->ExecuteInternal());
	this->isExecuting = false;
	this->isOperating = false;
}

void TacticModeSystem::PopCommand()
{
	this->commandQueue.back()->SetIsDone(true);
}

void TacticModeSystem::ClearCommand()
{
	for (auto& each : this->commandQueue)
	{
		each->HidePreviewMesh();
	}

	this->commandQueue.clear();
}

yunutyEngine::coroutine::Coroutine TacticModeSystem::ExecuteInternal()
{
	for (auto& each : commandQueue)
	{
		each->Execute();
		while (!each->IsDone())
		{
			co_await std::suspend_always();
		}
		each->HidePreviewMesh();
	}

	this->commandQueue.clear();
}
