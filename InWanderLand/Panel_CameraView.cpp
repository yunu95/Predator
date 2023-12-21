#include "Panel_CameraView.h"

#include "imgui.h"

namespace application
{
	namespace editor
	{
		CameraViewPanel::CameraViewPanel()
		{

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
	}
}
