#pragma once
/// 2024. 05. 16 이윤우
/// 스테이지 전환 Action 입니다.

#include "IAction.h"

namespace application
{
    class Action_EngageStage
        : public IAction
    {
    public:
        DEFINE_ACTION(EngageStage)

        Action_EngageStage() = default;
        Action_EngageStage(int stageIndex);

        virtual CoroutineObject<void> DoAction() override;

        void SetStageIndex(int index);

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        int index{ 0 };
    };
}
