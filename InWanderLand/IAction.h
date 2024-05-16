/// 2024. 04. 05 김상준
/// Script 에서 실행할 단위가 되는 Action 의 Interface 입니다.
/// IAction 은 CoroutineObject 를 반환하는 Coroutine 함수
/// DoAction 을 작성해야만 합니다.

#pragma once

#include "Storable.h"
#include "Identifiable.h"
#include "CoroutineObject.h"

#include "IObserver.h"

#include "imgui.h"
#include "imgui_Utility.h"

#include <string>

namespace application
{
    enum class ActionType
    {
        None,
        Example,
        WaitForSeconds,
        WaitForRealSeconds,
        CinematicModeChange,
        CameraChangeView,
        CameraSaveView,
        CameraLoadView,
        CinematicFadeIn,
        CinematicFadeOut,
        WaitPreviousActionEnd,
        CameraRevert,
        TutorialModeChange,
        PlayManualDialogue,
        PlayTimedDialogue,
        UnitMove,
        UnitRotate,
        UnitRescale,
        UnitMoveWithRotateAndRescale,
        UnitRelocate,
        UnitPauseAll,
        PlayerSelect,
        BlockPlayerSwitch,
        BlockSkillCancel,
        BlockSkillSelection,
        AwaitSkillSelection,
        AwaitSkillActivation,
        SetTimeScale,
        PauseWaves,
        EngageStage,
    };

    struct IAction
        : public Identifiable, public Storable, public IObserver
    {
        friend class Script;

        /// Observer 로 사용할 경우, 소멸자에서
        /// Target 의 Remove 를 호출해야 합니다.
        virtual ~IAction() = default;

        /// dynamic_cast 가 아닌 switch case 로 동작하기 위한 함수입니다.
        virtual ActionType GetType() const = 0;

        /// Editor 에서 사용하기 위해 Type 을 String 으로 반환하는 함수입니다.
        virtual std::string GetTypeName() const = 0;

        /// Coroutine 으로 실행할 함수입니다.
        virtual CoroutineObject<void> DoAction() = 0;

        /// pointer 를 내부에서 할당하는 등 특정 데이터를
        /// 세팅하지 않을 경우에 문제가 된다면 해당 데이터를
        /// 파악하여 Valid 를 return 하는 함수를 override 해야합니다.
        virtual bool IsValid() { return true; }

        /// 특별히 관찰할 대상이 있고, 그 대상으로부터 처리할 이벤트가 있을 경우,
        /// 해당 함수를 작성해야 합니다.
        virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) {}
    };
}

/// ImGui_DrawDataPopup 함수를 작성하여 Editor 에서 데이터 편집 기능을
/// 제공하여야 합니다.
#define DEFINE_ACTION(Class) \
virtual ActionType GetType() const override { return ActionType::Class; } \
virtual std::string GetTypeName() const override { return #Class; } \
static void ImGui_DrawDataPopup(Action_##Class* data);