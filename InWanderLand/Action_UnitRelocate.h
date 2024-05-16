#pragma once
/// 2024. 05. 16 이윤우
// 특정 유닛을 다른 지점으로 순간이동시킵니다.

#pragma once

#include "IAction.h"

#include "PlayableComponent.h"

#include "YunuGraphicsInterface.h"

namespace yunutyEngine
{
    class GameObject;
}

namespace application
{
    namespace editor
    {
        class Module_ScriptEditor;
        class UnitData;
    }

    class Action_UnitRelocate
        : public IAction
    {
        friend class editor::Module_ScriptEditor;

    public:
        DEFINE_ACTION(UnitRelocate)

            virtual ~Action_UnitRelocate();

        virtual CoroutineObject<void> DoAction() override;

        virtual bool IsValid() override;

        void SetTargetUnit(editor::UnitData* unit);
        void SetDestinationUnit(editor::UnitData* unit);

        virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        editor::UnitData* targetUnit = nullptr;
        editor::UnitData* destinationUnit = nullptr;
        bool isEditing = false;
    };
}
