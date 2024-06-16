/// 2024. 06. 16 김상준
/// Peace 상태로 진입하는 Action 입니다. 

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

	class Action_EngagePeace
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(EngagePeace)

			virtual CoroutineObject<void> DoAction() override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;
	};
}
