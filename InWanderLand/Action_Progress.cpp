#include "Action_Progress.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "ProgressManager.h"

namespace application
{
	CoroutineObject<void> Action_ProgressSave::DoAction()
	{
		ProgressManager::SingleInstance().NotifyCurrentProgressSaveAll();
		co_return;
	}

	bool Action_ProgressSave::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_ProgressSave::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_ProgressSave::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_ProgressSave::PostDecoding(const json& data)
	{
		return true;
	}

    void Action_ProgressSave::ImGui_DrawDataPopup(Action_ProgressSave* data)
    {

    }

	CoroutineObject<void> Action_ProgressLoad::DoAction()
	{
		ProgressManager::SingleInstance().NotifyRecoveryAll();
		co_return;
	}

	bool Action_ProgressLoad::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_ProgressLoad::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_ProgressLoad::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_ProgressLoad::PostDecoding(const json& data)
	{
		return true;
	}

	void Action_ProgressLoad::ImGui_DrawDataPopup(Action_ProgressLoad* data)
	{

	}
}
