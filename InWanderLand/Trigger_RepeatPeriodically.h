/// 2024. 04. 16 김상준
/// 게임 진행에 따라 주기적으로 호출되는 Trigger 입니다.

#pragma once

#include "ITrigger.h"

namespace application
{
	class Trigger_RepeatPeriodically
		: public ITrigger
	{
	public:
		DEFINE_TRIGGER(RepeatPeriodically)

		virtual void LinkCallback() override;

		void SetRepeatTime(float time);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		float repeatTime = 0;
	};

	class Trigger_RepeatPeriodicallyRealTime
		: public ITrigger
	{
	public:
		DEFINE_TRIGGER(RepeatPeriodicallyRealTime)

		virtual void LinkCallback() override;

		void SetRepeatTime(float time);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		float repeatTime = 0;
	};
}

