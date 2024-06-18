#include "Action_EngagePeace.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "PlayerController.h"

namespace application
{
	CoroutineObject<void> Action_EngagePeace::DoAction()
	{
		PlayerController::Instance().SetStateInAction(PlayerController::State::Peace,true);
		co_return;
	}

	void Action_EngagePeace::ImGui_DrawDataPopup(Action_EngagePeace* data)
	{

	}

	bool Action_EngagePeace::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_EngagePeace::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_EngagePeace::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_EngagePeace::PostDecoding(const json& data)
	{
		return true;
	}
}