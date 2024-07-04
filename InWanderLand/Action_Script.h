/// 2024. 07. 04 김상준
/// Script 에 관련된 Action 목록입니다.

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
    class Script;

    namespace editor
    {
        class Module_ScriptEditor;
    }

    /// 특정 Script 의 Trigger 효과를 발생하는 Action 입니다.
    class Action_PullScriptTrigger
        : public IAction
    {
        friend class editor::Module_ScriptEditor;

    public:
        DEFINE_ACTION(PullScriptTrigger)

        virtual ~Action_PullScriptTrigger();

        virtual CoroutineObject<void> DoAction() override;

        virtual bool IsValid() override;

        void SetTargetScript(Script* script);

        virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        Script* targetScript = nullptr;
    };
}
