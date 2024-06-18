#include "Action_RequestPeace.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "PlayerController.h"

namespace application
{
	CoroutineObject<void> Action_RequestPeace::DoAction()
	{
		PlayerController::Instance().SetState(PlayerController::State::Peace);
		co_return;
	}

	void Action_RequestPeace::ImGui_DrawDataPopup(Action_RequestPeace* data)
	{

	}

	bool Action_RequestPeace::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_RequestPeace::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_RequestPeace::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_RequestPeace::PostDecoding(const json& data)
	{
		return true;
	}
}