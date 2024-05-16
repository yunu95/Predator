#pragma once
/// 2024. 05. 16 이윤우
// 특정 스킬, 혹은 특정 스킬을 제외한 나머지 모든 스킬의 사용진입을 막습니다.
// 스킬 인덱스는 1부터 시작하며, 1은 로빈 Q, 2는 로빈 E, 3은 우르술라 Q, 4는 우르술라 E, 5는 헨젤 Q, 6은 헨젤 E입니다.

#include "IAction.h"

namespace application
{
    class Action_BlockSkillSelection
        : public IAction
    {
    public:
        DEFINE_ACTION(BlockSkillSelection)

            Action_BlockSkillSelection() = default;
        Action_BlockSkillSelection(int skillIndex);

        virtual CoroutineObject<void> DoAction() override;

        void SetSkillIndex(int index);

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        bool blocking{ true };
        bool applyExceptTarget{ false };
        int index{ 0 };
    };
}
