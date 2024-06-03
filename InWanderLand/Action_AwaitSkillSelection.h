#pragma once
/// 2024. 05. 16 이윤우
// 스킬 사용 단축키를 누르거나, 스킬 버튼을 클릭해 플레이어가 스킬 발동을 시도할 때까지 대기합니다.
// 대기하는 대상이 되는 스킬은 스킬 인덱스를 통해 지정할 수 있습니다.
// 스킬 인덱스는 1부터 시작하며, 1은 로빈 Q, 2는 로빈 E, 3은 우르술라 Q, 4는 우르술라 E, 5는 헨젤 Q, 6은 헨젤 E입니다.

#include "IAction.h"
#include "SkillType.h"

namespace application
{
    class Action_AwaitSkillSelection
        : public IAction
    {
    public:
        DEFINE_ACTION(AwaitSkillSelection)

            Action_AwaitSkillSelection() = default;
        Action_AwaitSkillSelection(SkillType::Enum skillType);

        virtual CoroutineObject<void> DoAction() override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        POD_Enum<SkillType::Enum> skillType;
    };
};
