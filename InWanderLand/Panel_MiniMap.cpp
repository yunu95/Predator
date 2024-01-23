#include "InWanderLand.h"
#include "Panel_MiniMap.h"
#include "Application.h"

#include "imgui.h"

namespace application
{
	namespace editor
	{
		MiniMapPanel::MiniMapPanel()
		{

		}

		MiniMapPanel::~MiniMapPanel()
		{

		}

		void MiniMapPanel::Initialize()
		{

		}

		void MiniMapPanel::Update(float ts)
		{

		}

		void MiniMapPanel::GUIProgress()
		{
			ImGui::Begin("MiniMap", 0, ImGuiWindowFlags_NoBringToFrontOnFocus);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역


			ImGui::End();
		}

		void MiniMapPanel::Finalize()
		{

		}
	}
}
