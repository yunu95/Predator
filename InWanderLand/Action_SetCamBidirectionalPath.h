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

    class Action_SetCamBidirectionalPath
        : public IAction
    {
        friend class editor::Module_ScriptEditor;

    public:
        DEFINE_ACTION(SetCamBidirectionalPath)

            virtual CoroutineObject<void> DoAction() override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        float pathX = 1;
        float pathZ = 1;
        bool applyZoomFactor = false;
        bool constrainCamUpdate = false;
        float zoomFactor = 10;
    };
}
