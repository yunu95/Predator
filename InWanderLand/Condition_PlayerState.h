/// 2024. 06. 21 김상준
/// Player State 상태에 따라 Action 을 수행하도록 제한하는 조건입니다.

#pragma once

#include "ICondition.h"

namespace application
{
	class Condition_PlayerState
		: public ICondition
	{
	public:
		DEFINE_CONDITION(PlayerState)

		virtual bool IsConditionMet() override;

		void SetState(int state);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		int state = 0;
	};
}

