/// 2024. 06. 10 김상준
/// Boss 에 관련된 Action 목록입니다. 

#pragma once

#include "IAction.h"

#include "PlayableComponent.h"

#include "YunuGraphicsInterface.h"

namespace yunutyEngine
{
	class GameObject;
}

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
	}

	/// 배치된 보스의 등장 연출을 위한 Action 입니다.
	class Action_BossAppear
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(BossAppear)

		virtual CoroutineObject<void> DoAction() override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;
	};
}
