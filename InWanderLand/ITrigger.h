/// 2024. 04. 05 김상준
/// Script 에서 실행을 유발하게 되는 Trigger 의 Interface 입니다.
/// 구체화된 Trigger는 Singleton 객체여야만합니다.

#pragma once

#include "Storable.h"
#include "Identifiable.h"

#include "IObserver.h"

#include "imgui.h"
#include "imgui_Utility.h"

#include <functional>
#include <string>

namespace application
{
    enum class TriggerType
    {
        None,
        GameStart,
        EnterRegion,
        LeaveRegion,
        RepeatPeriodically,
        RepeatPeriodicallyRealTime,
        UnitAppear,
        UnitDie,
        InteractableTriggerEnter,
        InteractableTriggerExit,
        StartWave,
        EndWave,
        StageStart,
        Shortcut,
    };

    struct ITrigger
        : public Identifiable, public Storable, public IObserver
    {
        friend class Script;

        /// Observer 로 사용할 경우, 소멸자에서
        /// Target 의 Remove 를 호출해야 합니다.
        virtual ~ITrigger() = default;

        /// dynamic_cast 가 아닌 switch case 로 동작하기 위한 함수입니다.
        virtual TriggerType GetType() const = 0;

        /// Editor 에서 사용하기 위해 Type 을 String 으로 반환하는 함수입니다.
        virtual std::string GetTypeName() const = 0;

        /// 스크립트가 인게임 요소로 활성화될 때 트리거들의 설정에 맞게 인게임 객체들의
        /// 콜백 functor들과 PullTrigger 함수 호출을 연계합니다.
        virtual void LinkCallback() = 0;

        /// ITrigger 를 상속받는 구체화된 Trigger 는 callable 을 등록할 수 있게 마련하고,
        /// Trigger 가 유발될 때, 해당 callable 을 일괄적으로 호출할 수 있어야만 합니다.
        bool AddFunction(const std::function<void()>& func)
        {
            doList.push_back(func);
            return true;
        }

        void Clear()
        {
            doList.clear();
        }

        /// doList 에 등록된 functor 를 일괄적으로 실행합니다.
        /// ITrigger 를 상속받은 구체화된 Trigger 쪽에서 조건을 만족했을 때,
        /// 해당 함수를 호출해야 합니다.
        virtual void PullTrigger() const
        {
            for (auto& each : doList)
            {
                each();
            }
        }

        /// pointer 를 내부에서 할당하는 등 특정 데이터를
        /// 세팅하지 않을 경우에 문제가 된다면 해당 데이터를
        /// 파악하여 Valid 를 return 하는 함수를 override 해야합니다.
        virtual bool IsValid() { return true; }

        /// 특별히 관찰할 대상이 있고, 그 대상으로부터 처리할 이벤트가 있을 경우,
        /// 해당 함수를 작성해야 합니다.
        virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) {}

    private:
        std::vector<std::function<void()>> doList;
    };
}

/// ImGui_DrawDataPopup 함수를 작성하여 Editor 에서 데이터 편집 기능을
/// 제공하여야 합니다.
#define DEFINE_TRIGGER(Class) \
virtual TriggerType GetType() const override { return TriggerType::Class; } \
virtual std::string GetTypeName() const override { return #Class; } \
static void ImGui_DrawDataPopup(Trigger_##Class* data);
