/// 2024. 04.18 김상준
/// 이전 Action 이 종료될 때까지 대기하는 Action 입니다.

#pragma once

#include "IAction.h"

namespace application
{
	class Action_WaitPreviousActionEnd
		: public IAction
	{
	public:
		DEFINE_ACTION(WaitPreviousActionEnd)

		virtual CoroutineObject<void> DoAction() override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;
	};
}
