/// 2024. 07. 18 김상준
/// Progress 에 관련된 Action 목록입니다. 

#pragma once

#include "IAction.h"

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

    /// 현재 게임의 Progress 를 기록하는 함수입니다.
    class Action_ProgressSave
        : public IAction
    {
        friend class editor::Module_ScriptEditor;

    public:
        DEFINE_ACTION(ProgressSave)

        virtual CoroutineObject<void> DoAction() override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
    };

    /// 현재 게임의 Save 된 Progress 를 Load 하는 함수입니다.
    class Action_ProgressLoad
        : public IAction
    {
        friend class editor::Module_ScriptEditor;

    public:
        DEFINE_ACTION(ProgressLoad)

        virtual CoroutineObject<void> DoAction() override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
    };
}
