/// 2024. 04. 08 김상준
/// 정해진 시간 동안 진행하지 않는 Action 입니다.
/// timescale 에 대한 두 가지 버전을 제공합니다.

#pragma once

#include "IAction.h"

namespace application
{
	class Action_WaitForSeconds
		: public IAction
	{
	public:
		DEFINE_ACTION(WaitForSeconds)

		virtual CoroutineObject<void> DoAction() override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		Action_WaitForSeconds(const float& waitTime);

	private:
		float waitTime = 0;
	};

	class Action_WaitForRealSeconds
		: public IAction
	{
	public:
		DEFINE_ACTION(WaitForRealSeconds)

		virtual CoroutineObject<void> DoAction() override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		Action_WaitForRealSeconds(const float& waitTime);

	private:
		float waitTime = 0;
	};
}

