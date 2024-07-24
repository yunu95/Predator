/// 2024. 07. 24 이윤우
/// 숏컷을 눌렀을 때 유발되는 트리거

#pragma once

#include "ITrigger.h"
#include "ShortcutType.h"

namespace application
{
    class Trigger_Shortcut
        : public ITrigger
    {
    public:
        DEFINE_TRIGGER(Shortcut)

            virtual void LinkCallback() override;

        virtual bool PreEncoding(json& data) const override;
        virtual bool PostEncoding(json& data) const override;
        virtual bool PreDecoding(const json& data) override;
        virtual bool PostDecoding(const json& data) override;

    private:
        POD_Enum<ShortcutType::Enum> shortcutType;
    };
}

