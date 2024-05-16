#pragma once
/// 2024. 05. 16 이윤우
// 스킬 시전이 시작될 때까지 대기합니다.
// 대기하는 대상이 되는 스킬은 스킬 인덱스를 통해 지정할 수 있습니다.
// 스킬 인덱스는 1부터 시작하며, 1은 로빈 Q, 2는 로빈 E, 3은 우르술라 Q, 4는 우르술라 E, 5는 헨젤 Q, 6은 헨젤 E입니다.

#include "IAction.h"

namespace application
{
    class Action_AwaitSkillActivation
        : public IAction
    {
    public:
        DEFINE_ACTION(AwaitSkillActivation)

        Action_AwaitSkillActivation() = default;
        Action_AwaitSkillActivation(int skillIndex);

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
