/*****************************************************************
 * \file   YunutyCycle.h
 * \brief  
 * 
 * \author KOCCA23
 * \date   September 2023
 *********************************************************************/
#pragma once
#include <thread>
#include "Object.h"
#include <functional>
#include <mutex>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    class Component;
    /**
     * \brief 사이클은 게임 엔진의 메인스레드의 구동을 제어하는 싱글톤 클래스이다.
     */
    class YUNUTY_API YunutyCycle : Object
    {
    private:
        thread updateThread;
        bool isGameRunning{ false };
        bool isUpdating{ false };

        void ActiveComponentsDo(function<void(Component*)> todo);
        void ActiveComponentsDo(void (Component::* method)());
        vector<Component*> GetActiveComponents();
        vector<GameObject*> GetGameObjects(bool onlyActive = true);
        vector<std::function<void()>> afterUpdateActions;
        std::mutex actionReservationMutex;
        
        static void UpdateComponent(Component* component);
        static void StartComponent(Component* component);
        void ThreadFunction();
    protected:
        static YunutyCycle* _instance;
        YunutyCycle();
        virtual ~YunutyCycle();
        virtual void ThreadUpdate();
    public:
        // 수동으로 렌더링을 구동할 것 같으면 false로 두면 된다.
        bool autoRendering = true;
        // yunuty 라이프사이클을 실행시키는 뮤텍스.
        // 외부에서 락을 걸면 게임엔진 스레드의 업데이트 사이클이 돌고 있지 않음이 보장된다.
        std::mutex updateMutex;
        // yunuty 업데이트 라이프사이클을 실행하기 전 거치는 뮤텍스.
        // 외부에서 락을 걸면 게임엔진 스레드의 업데이트 사이클이 한번 더 돌지 않을 것임이 보장된다.
        std::mutex preUpdateMutex;
        // 예외가 발생했을때 예외를 던지는 대신 이 함수를 호출한다. 함수가 등록되어 있지 않으면 예외를 던진다.
        std::function<void(const std::exception& e)> onExceptionThrown;
        static YunutyCycle& SingleInstance();
        virtual void Initialize();
        virtual void Release();
        void Play();
        void Stop();
        void Pause();
        // 엔진을 활용하는 스레드와 동시성 문제가 생길때 사용하는 함수
        // 업데이트가 끝난후 동작시킬 함수를 등록한다. 한번 실행된 함수는 그대로 삭제된다.
        void ReserveActionAfterUpdate(std::function<void()> action);
        void SetMaxFrameRate();
        bool IsGameRunning();
        bool IsUpdating();
    };
}
