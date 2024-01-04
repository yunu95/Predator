#include "Panel_Inspector.h"

#include "imgui.h"

namespace application
{
	namespace editor
	{
		InspectorPanel::InspectorPanel()
		{

		}

		InspectorPanel::~InspectorPanel()
		{

		}

		void InspectorPanel::Initialize()
		{

		}

		void InspectorPanel::Update(float ts)
		{

		}

		void InspectorPanel::GUIProgress()
		{
			ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoBringToFrontOnFocus);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역


			ImGui::End();
		}

		void InspectorPanel::Finalize()
		{

		}
	}
}
