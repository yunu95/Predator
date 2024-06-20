#pragma once
/// 2024. 05. 16 이윤우
// 특정 스킬, 혹은 특정 스킬을 제외한 나머지 모든 스킬의 사용진입을 막습니다.
// SkillType::Enum 참고

#include "IAction.h"

#include "IAction.h"
#include "SkillType.h"

namespace application
{
    class Action_BlockSkillSelection
        : public IAction
    {
    public:
        DEFINE_ACTION(BlockSkillSelection)

            Action_BlockSkillSelection() = default;
        Action_BlockSkillSelection(SkillType::Enum skillType);

        virtual CoroutineObject<void> DoAction() override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        POD_Enum<SkillType::Enum> skillType;
        bool blocking;
        bool applyExceptTarget;
    };
};
