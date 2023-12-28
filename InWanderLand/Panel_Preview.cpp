#include "Panel_Preview.h"

#include "imgui.h"

namespace application
{
	namespace editor
	{
		PreviewPanel::PreviewPanel()
		{

		}

		PreviewPanel::~PreviewPanel()
		{

		}

		void PreviewPanel::Initialize()
		{

		}

		void PreviewPanel::Update(float ts)
		{

		}

		void PreviewPanel::GUIProgress()
		{
			ImGui::Begin("Preview");

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역


			ImGui::End();
		}

		void PreviewPanel::Finalize()
		{

		}
	}
}
