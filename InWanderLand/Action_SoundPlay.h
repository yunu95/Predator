#pragma once
/// 2024. 05. 09 이윤우
/// 효과음을 재생합니다.
#pragma once

#include <string>
#include "IAction.h"

namespace application
{
    /// CinematicMode 전환 Action 입니다.
    class Action_SoundPlay
        : public IAction
    {
    public:
        DEFINE_ACTION(SoundPlay)

        virtual CoroutineObject<void> DoAction() override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        std::string soundPath;
    };
}
