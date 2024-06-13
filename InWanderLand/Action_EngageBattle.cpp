#include "Action_EngageBattle.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "PlayerController.h"

namespace application
{
	CoroutineObject<void> Action_EngageBattle::DoAction()
	{
		PlayerController::Instance().SetState(PlayerController::State::Battle);
		co_return;
	}

	void Action_EngageBattle::ImGui_DrawDataPopup(Action_EngageBattle* data)
	{

	}

	bool Action_EngageBattle::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_EngageBattle::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_EngageBattle::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_EngageBattle::PostDecoding(const json& data)
	{
		return true;
	}
}