#pragma once
/// 2024. 05. 16 이윤우
/// 게임의 진행속도, 즉 TimeScale을 변경하는 액션입니다.

#pragma once

#include "IAction.h"

namespace application
{
    class Action_SetTimeScale
        : public IAction
    {
    public:
        DEFINE_ACTION(SetTimeScale)

            Action_SetTimeScale() = default;
        Action_SetTimeScale(float time);

        virtual CoroutineObject<void> DoAction() override;

        void SetTimeScale(float scale);

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        float timeScale = 1;
    };
}
