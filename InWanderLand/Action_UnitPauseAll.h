/// 2024. 05. 16 이윤우
// 모든 유닛들의 동작을 일시정지시킨다. 다만 플레이어의 경우 스킬은 쓸 수 있어야 한다.
// usecase : 튜토리얼에서 스킬의 사용법을 알려주기 위해 잠깐 적들과 아군의 움직임을 멈추고 싶을 때

#pragma once

#include "IAction.h"

namespace application
{
    class Action_UnitPauseAll
        : public IAction
    {
    public:
        DEFINE_ACTION(UnitPauseAll)

            Action_UnitPauseAll() = default;
        Action_UnitPauseAll(bool pause);

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
