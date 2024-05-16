#pragma once
/// 2024. 05. 16 이윤우
// 유닛을 강제로 선택시킵니다.
// 플레이어 인덱스는 1부터 시작하며, 1은 로빈, 2은 우르술라, 3는 헨젤입니다.

#include "IAction.h"

namespace application
{
    class Action_PlayerSelect
        : public IAction
    {
    public:
        DEFINE_ACTION(PlayerSelect)

            Action_PlayerSelect() = default;
        Action_PlayerSelect(int playerIndex);

        virtual CoroutineObject<void> DoAction() override;

        void SetSkillIndex(int index);

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        int index{ 0 };
    };
}
