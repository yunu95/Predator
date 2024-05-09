/// 2024. 05. 09 이윤우
/// 대화창을 넘기기 위해서 유저가 매번 클릭을 해야 하는 대화창을 출력하기 위한 액션입니다.
#pragma once

#include <string>
#include "IAction.h"

namespace application
{
    /// CinematicMode 전환 Action 입니다.
    class Action_PlayManualDialogue
        : public IAction
    {
    public:
        DEFINE_ACTION(PlayManualDialogue)

            virtual CoroutineObject<void> DoAction() override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

        std::string dialogueKeyID;
    private:
        //bool cinematicMode = false;
    };
}
