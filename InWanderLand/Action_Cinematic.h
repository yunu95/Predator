#pragma once

#include "IAction.h"

namespace application
{
	/// CinematicMode 전환 Action 입니다.
	class Action_CinematicModeChange
		: public IAction
	{
	public:
		DEFINE_ACTION(CinematicModeChange)

		virtual CoroutineObject<void> DoAction() override;

		void SetCinematicMode(bool cinematicMode);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		bool cinematicMode = false;
	};
}
