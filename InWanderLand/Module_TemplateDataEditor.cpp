#include "Module_TemplateDataEditor.h"

#include "imgui.h"

namespace application
{
	namespace editor
	{
		Module_TemplateDataEditor::Module_TemplateDataEditor()
		{

		}

		Module_TemplateDataEditor::~Module_TemplateDataEditor()
		{

		}

		void Module_TemplateDataEditor::Initialize()
		{

		}

		void Module_TemplateDataEditor::Update(float ts)
		{

		}

		void Module_TemplateDataEditor::GUIProgress()
		{
			CheckActivation();

			ImGui::Begin("TemplateData Editor", &activation);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역


			ImGui::End();
		}

		void Module_TemplateDataEditor::Finalize()
		{

		}
	}
}

