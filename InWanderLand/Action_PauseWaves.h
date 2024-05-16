/// 2024. 05. 16 이윤우
// 웨이브가 더이상 업데이트되지 않도록 모두 중지시키거나 재개시킨다.
// usecase : 튜토리얼에서 스킬의 사용법을 알려주기 위해 잠깐 적들과 아군의 움직임을 멈추고 싶을 때

#pragma once

#include "IAction.h"

namespace application
{
    class Action_PauseWaves
        : public IAction
    {
    public:
        DEFINE_ACTION(PauseWaves)

            Action_PauseWaves() = default;
        Action_PauseWaves(bool pause);

        virtual CoroutineObject<void> DoAction() override;

        void SetPause(bool pause);

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        bool pause{ true };
    };
}
