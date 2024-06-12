#include "Action_Boss.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "BossController.h"

namespace application
{
	CoroutineObject<void> Action_BossAppear::DoAction()
	{
		BossController::Instance().BossAppear();
		co_return;
	}

	void Action_BossAppear::ImGui_DrawDataPopup(Action_BossAppear* data)
	{
		
	}

	bool Action_BossAppear::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_BossAppear::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_BossAppear::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_BossAppear::PostDecoding(const json& data)
	{
		return true;
	}
}