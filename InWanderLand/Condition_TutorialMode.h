/// 2024. 05. 09 김상준
/// TutorialMode 일 경우에만 Action 을 수행하도록 제한하는 조건입니다.

#pragma once

#include "ICondition.h"

namespace application
{
	class Condition_TutorialModeOn
		: public ICondition
	{
	public:
		DEFINE_CONDITION(TutorialModeOn)

		virtual bool IsConditionMet() override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;
	};

	class Condition_TutorialModeOff
		: public ICondition
	{
	public:
		DEFINE_CONDITION(TutorialModeOff)

		virtual bool IsConditionMet() override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;
	};
}

