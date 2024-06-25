/// 2024. 06. 25 김상준
/// Stage 상태에 따라 Action 을 수행하도록 제한하는 조건입니다.

#pragma once

#include "ICondition.h"

namespace application
{
	class Condition_Stage
		: public ICondition
	{
	public:
		DEFINE_CONDITION(Stage)

		virtual bool IsConditionMet() override;

		void SetStageIndex(int index);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		int index = 1;
	};
}

