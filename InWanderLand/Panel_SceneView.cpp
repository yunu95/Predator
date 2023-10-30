#include "Panel_SceneView.h"

#include "imgui.h"

namespace Application
{
	namespace Editor
	{
		std::unique_ptr<SceneViewPanel> SceneViewPanel::instance = nullptr;

		SceneViewPanel& SceneViewPanel::GetInstance()
		{
			if (instance == nullptr)
			{
				instance = std::unique_ptr<SceneViewPanel>(new SceneViewPanel());
			}

			return *instance;
		}

		SceneViewPanel::~SceneViewPanel()
		{

		}

		void SceneViewPanel::Initialize()
		{

		}

		void SceneViewPanel::Update(float ts)
		{

		}

		void SceneViewPanel::GUIProgress()
		{
			ImGui::Begin("SceneView");

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역


			ImGui::End();
		}

		void SceneViewPanel::Finalize()
		{

		}

		SceneViewPanel::SceneViewPanel()
		{

		}
	}
}
