#include "Action_WaitPreviousActionEnd.h"

namespace application
{
	CoroutineObject<void> Action_WaitPreviousActionEnd::DoAction()
	{
		/// 해당 Action 은 Script 에서 예외처리로 적용합니다.
		co_return;
	}

	void Action_WaitPreviousActionEnd::ImGui_DrawDataPopup(Action_WaitPreviousActionEnd* data)
	{

	}

	bool Action_WaitPreviousActionEnd::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_WaitPreviousActionEnd::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_WaitPreviousActionEnd::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_WaitPreviousActionEnd::PostDecoding(const json& data)
	{
		return true;
	}
}
