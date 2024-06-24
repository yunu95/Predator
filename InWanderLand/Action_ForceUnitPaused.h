#pragma once
/// 2024. 06. 23 이윤우
// 스킬 시전이 종료될 때까지 대기합니다.
// 대기하는 대상이 되는 스킬은 스킬 인덱스를 통해 지정할 수 있습니다.
// 스킬 인덱스는 1부터 시작하며, 1은 로빈 Q, 2는 로빈 E, 3은 우르술라 Q, 4는 우르술라 E, 5는 헨젤 Q, 6은 헨젤 E입니다.

#include "IAction.h"
#include "SkillType.h"

namespace application
{
    namespace editor
    {
        class Module_ScriptEditor;
        class UnitData;
    }

    class Action_ForceUnitPaused
        : public IAction
    {
        friend class editor::Module_ScriptEditor;

    public:
        DEFINE_ACTION(ForceUnitPaused)

        Action_ForceUnitPaused() = default;

        virtual CoroutineObject<void> DoAction() override;
        void SetTargetUnit(editor::UnitData* unit);

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;
        virtual bool IsValid() override;
        virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

    private:
        editor::UnitData* targetUnit = nullptr;
        bool isEditing = false;
        bool withdrawRequest = false;
    };
};