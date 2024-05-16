#pragma once
/// 2024. 05. 16 이윤우
// 시전하려고 하는 스킬을 취소하는 것을 막는다.

#pragma once

#include "IAction.h"

namespace application
{
    class Action_BlockSkillCancel
        : public IAction
    {
    public:
        DEFINE_ACTION(BlockSkillCancel)

            Action_BlockSkillCancel() = default;
        Action_BlockSkillCancel(bool block);

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
