#include "Panel_CameraView.h"

#include "imgui.h"

namespace Application
{
	namespace Editor
	{
		std::unique_ptr<CameraViewPanel> CameraViewPanel::instance = nullptr;

		CameraViewPanel& CameraViewPanel::GetInstance()
		{
			if (instance == nullptr)
			{
				instance = std::unique_ptr<CameraViewPanel>(new CameraViewPanel());
			}

			return *instance;
		}

		CameraViewPanel::~CameraViewPanel()
		{

		}

		void CameraViewPanel::Initialize()
		{

		}

		void CameraViewPanel::Update(float ts)
		{

		}

		void CameraViewPanel::GUIProgress()
		{
			ImGui::Begin("CameraView");

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역


			ImGui::End();
		}

		void CameraViewPanel::Finalize()
		{

		}

		CameraViewPanel::CameraViewPanel()
		{

		}
	}
}
