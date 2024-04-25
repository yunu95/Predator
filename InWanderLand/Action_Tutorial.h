/// 2024. 04. 22 김상준
/// Tutorial 에 관련된 Action 목록입니다.


#pragma once

#include "IAction.h"

namespace application
{
	/// TutorialMode 전환 Action 입니다.
	class Action_TutorialModeChange
		: public IAction
	{
	public:
		DEFINE_ACTION(TutorialModeChange)

		virtual CoroutineObject<void> DoAction() override;

		void SetTutorialMode(bool tutorialMode);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		bool tutorialMode = false;
	};
}
