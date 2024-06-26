#include "Module_Skill.h"
#include "InWanderLand.h"
#include "Application.h"
#include "SkillManager.h"
#include "imgui.h"
#include "imgui_ModuleData.h"

#include "SkillList.h"

namespace application
{
	namespace editor
	{
		Module_Skill::Module_Skill()
		{

		}

		Module_Skill::~Module_Skill()
		{

		}

		void Module_Skill::Initialize()
		{

		}

		void Module_Skill::Update(float ts)
		{

		}

		void Module_Skill::GUIProgress()
		{
			CheckActivation();

			ImGui::Begin("Skill Editor", &activation);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역
			DrawLayout();

			ImGui::End();
		}

		void Module_Skill::Finalize()
		{

		}

		void Module_Skill::DrawLayout()
		{
			static auto spacing = ImGui::GetStyle().ItemSpacing;

			static int idx = 0;
			imgui::BeginSection_2Col(idx, "Skill Module", ImGui::GetContentRegionAvail().x, 0.2f);
			imgui::DropdownEnum_2Col("Selected Skill", selectedSkill);
			imgui::ShiftCursorY(10);
			if (selectedSkill.enumValue != (int)SkillType::NONE)
			{
				DrawList();
			}
			imgui::EndSection();
		}

		void Module_Skill::DrawList()
		{
			Skill::DrawSkillPod((SkillType::Enum)selectedSkill.enumValue);
		}
	}
}

