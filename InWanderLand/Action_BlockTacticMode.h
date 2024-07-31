/// 2024. 07. 29 박규현
// 전술모드 진입을 막습니다.
// usecase : 튜토리얼 도중 spacebar를 눌러도 전술모드에 진입하지 못하도록 하기 위해 쓰임.

#pragma once

#include "IAction.h"

namespace application
{
    class Action_BlockTacticMode
        : public IAction
    {
    public:
        DEFINE_ACTION(BlockTacticMode)

            Action_BlockTacticMode() = default;
        Action_BlockTacticMode(bool block);

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
