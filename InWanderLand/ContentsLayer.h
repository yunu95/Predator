/// 2023. 10. 11 김상준
/// Contents 제작을 위한 레이어

#pragma once

#include "Layer.h"
#include "UnitProductor.h"
#include "ContentsStopFlag.h"
#include "ContentsPlayFlag.h"
#include <functional>

namespace application
{
    namespace contents
    {
        class ContentsLayer
            : public Layer
        {
        private:
            static std::function<void()> testInitializer;
        public:
            static void SetInputControl(bool control);
            static bool GetInputControl();

            virtual void Initialize() override;
            //virtual void EventProgress() override;
            virtual void Update(float ts) override;
            virtual void GUIProgress() override;
            virtual void Finalize() override;

            void PlayContents(ContentsPlayFlag playFlag = ContentsPlayFlag::ImportUI);
            // 네비게이션 빌드에 겁나 많은 시간이 들어가므로 네비게이션 빌드만 따로 스레드로 빼고 싶을때 사용
            //coroutine::Coroutine BuildNavFieldCoroutine(ContentsPlayFlag playFlag = ContentsPlayFlag::ImportUI);
            void PauseContents();
            void ResumeContents();
            void StopContents(ContentsStopFlag playFlag = ContentsStopFlag::ClearUI);
            bool IsPlayingContents() const;

            bool playingContents{ false };
            bool isStoppedOnce{ false };
            // 테스트 코드에서 임의로 ContentsLayer의 Initialize 코드를 오버라이드 하고 싶을때 쓰이는 함수
            // 이 함수를 호출하면 YunutyCycle에서 발생한 예외를 잡아서 테스트 스레드에서 예외를 던집니다.
#ifdef GEN_TESTS
            static void AssignTestInitializer(std::function<void()> testInitializer);
#endif
        private:
            void ShortcutInit();
        };
    }
}
