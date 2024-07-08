#include "Module_SoundEditor.h"

#include "InWanderLand.h"
#include "Application.h"
#include "imgui.h"
#include "imgui_ModuleData.h"

namespace application
{
	namespace editor
	{
		Module_SoundEditor::~Module_SoundEditor()
		{

		}

		void Module_SoundEditor::Initialize()
		{

		}

		void Module_SoundEditor::Update(float ts)
		{

		}

		void Module_SoundEditor::GUIProgress()
		{
			CheckActivation();

			ImGui::Begin("Sound Editor", &activation);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역
			DrawLayout();

			ImGui::End();
		}
		void Module_SoundEditor::Finalize()
		{

		}

		Module_SoundEditor::Module_SoundEditor()
		{

		}

		void Module_SoundEditor::DrawLayout()
		{
			static auto spacing = ImGui::GetStyle().ItemSpacing;

			static int idx = 0;
			imgui::BeginSection_2Col(idx, "Sound Module", ImGui::GetContentRegionAvail().x, 0.2f);
			imgui::ShiftCursorY(10);
			DrawList();
			imgui::EndSection();
		}

		void Module_SoundEditor::DrawList()
		{

		}
	}
}
