/// 2024. 05. 09 이윤우
/// 일정 시간동안 떠 있다가 알아서 사라지는 대화를 출력하는 Action 입니다.
#pragma once

#include <string>
#include "IAction.h"

namespace application
{
    /// CinematicMode 전환 Action 입니다.
    class Action_UISetActive
        : public IAction
    {
    public:
        DEFINE_ACTION(UISetActive)

            virtual CoroutineObject<void> DoAction() override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

        std::string uiName;
        bool active = true;
    private:
    };
}
