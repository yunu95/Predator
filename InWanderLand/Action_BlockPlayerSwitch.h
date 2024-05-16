/// 2024. 05. 16 이윤우
// 플레이어 선택을 바꾸는 것을 막는다.
// usecase : 튜토리얼에서 함부로 플레이어들을 바꾸지 못하게 하기 위해 쓰임.

#pragma once

#include "IAction.h"

namespace application
{
    class Action_BlockPlayerSwitch
        : public IAction
    {
    public:
        DEFINE_ACTION(BlockPlayerSwitch)

            Action_BlockPlayerSwitch() = default;
        Action_BlockPlayerSwitch(bool block);

        virtual CoroutineObject<void> DoAction() override;

        void SetBlock(bool block);

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        bool block{ true };
    };
}
