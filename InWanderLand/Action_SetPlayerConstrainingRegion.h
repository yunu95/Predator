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
        class RegionData;
    }

    class Action_SetPlayerConstrainingRegion
        : public IAction
    {
        friend class editor::Module_ScriptEditor;

    public:
        DEFINE_ACTION(SetPlayerConstrainingRegion)

            virtual ~Action_SetPlayerConstrainingRegion();

        virtual CoroutineObject<void> DoAction() override;

        virtual bool IsValid() override;

        void SetTargetRegion(editor::RegionData* region);

        virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        editor::RegionData* targetRegion = nullptr;
        bool doesConstrain = false;
        bool isEditing = false;
    };
}
