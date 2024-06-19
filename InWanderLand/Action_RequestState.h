/// 2024. 06. 12 김상준
/// Battle 상태로 진입하는 Action 입니다. 

#pragma once

#include "IAction.h"

#include "PlayableComponent.h"

#include "YunuGraphicsInterface.h"

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
	}

	class Action_RequestState
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(RequestState)

		virtual CoroutineObject<void> DoAction() override;

		void SetRequest(int request);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		int request = 0;
	};
}
