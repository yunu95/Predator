/// 2023.10. 11 김상준
/// Entry Point 가 되는 main 함수를 간결화시키고, 작업 영역을 구분하여 사용하기 위한
/// 프로세스의 단위체 역할을 하는 클래스 작성

#pragma once

#include "Layer.h"
#include "CommandManager.h"

#include <vector>
#include <memory>

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

	private:
		enum class LayerList
		{
			EditorLayer	= 0,
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
