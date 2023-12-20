/// 2023.10. 11 김상준
/// Entry Point 가 되는 main 함수를 간결화시키고, 작업 영역을 구분하여 사용하기 위한
/// 프로세스의 단위체 역할을 하는 클래스 작성

#pragma once

#include "Layer.h"
#include "CommandManager.h"

#include <vector>
#include <memory>
#include <functional>
#include <mutex>

namespace Application
{
    class Application
    {
    public:
        static Application& CreateApplication(int argc, char** argv);
        static Application& GetInstance();

        ~Application();

        /// 필수 요소
        void Initialize();		// 초기화
        void Run();				// 실행(루프)
        void Finalize();		// 마무리

        /// 기능 단위 요소
        void TurnOff();			// Run 상태일 때, 루프를 탈출함
        // 메인 스레드 루프가 끝나기 전에 실행할 함수를 등록합니다. 등록된 동작은 한번 실행후 사라집니다.
        // AddMainLoopTodo 함수는 메인 스레드 루프에 뮤텍스 락을 겁니다.
        // AddMainLoopTodo 함수의 매개변수로 등록된 동작 todo는 실행될 때 게임 엔진 루프에 뮤텍스 락을 겁니다.
        void AddMainLoopTodo(std::function<void()> todo);

    private:
        std::mutex loopTodoRegistrationMutex;
        // AddMainLoopTodo로 등록된 휘발성 콜백 함수들입니다.
        // 매 루프가 종료될 때 이 컨테이너에 실행 동작들이 담겨있다면 모두 실행하고 내용을 비웁니다.
        // 이 목록에 담긴 함수들이 실행되는 동안 게임 엔진 스레드는 동작을 정지합니다.
        std::vector<std::function<void()>> loopRegistrations;

        enum class LayerList
        {
            EditorLayer = 0,
            ContentsLayer,
        };

        static std::unique_ptr<Application> instance;

        Application(int argc, char** argv);
        Application(const Application& app) = delete;
        Application& operator=(const Application& app) = delete;

        bool isRunning = false;
        std::vector<Layer*> layers;

#ifdef _DEBUG
        Editor::CommandManager& cm = Editor::CommandManager::GetInstance();
#endif
    };
}
