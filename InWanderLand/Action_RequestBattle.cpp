#include "Action_RequestBattle.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "PlayerController.h"

namespace application
{
	CoroutineObject<void> Action_RequestBattle::DoAction()
	{
		PlayerController::Instance().SetState(PlayerController::State::Battle);
		co_return;
	}

	void Action_RequestBattle::ImGui_DrawDataPopup(Action_RequestBattle* data)
	{

	}

	bool Action_RequestBattle::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_RequestBattle::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_RequestBattle::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_RequestBattle::PostDecoding(const json& data)
	{
		return true;
	}
}