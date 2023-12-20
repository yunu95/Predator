#include "Panel_Inspector.h"

#include "imgui.h"

namespace application
{
	namespace editor
	{
		std::unique_ptr<InspectorPanel> InspectorPanel::instance = nullptr;

		InspectorPanel& InspectorPanel::GetInstance()
		{
			if (instance == nullptr)
			{
				instance = std::unique_ptr<InspectorPanel>(new InspectorPanel());
			}

			return *instance;
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
			ImGui::Begin("Inspector");

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역


			ImGui::End();
		}

		void InspectorPanel::Finalize()
		{

		}

		InspectorPanel::InspectorPanel()
		{

		}
	}
}
