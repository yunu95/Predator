#include "InWanderLand.h"
#include "Module_GlobalConstant.h"
#include "Application.h"
#include "TemplateDataManager.h"
#include "GlobalConstant.h"

namespace application
{
	namespace editor
	{
		Module_GlobalConstant::Module_GlobalConstant()
			: gc(GlobalConstant::GetSingletonInstance())
		{

		}

		Module_GlobalConstant::~Module_GlobalConstant()
		{

		}

		void Module_GlobalConstant::Initialize()
		{

		}

		void Module_GlobalConstant::Update(float ts)
		{

		}

		void Module_GlobalConstant::GUIProgress()
		{
			CheckActivation();

			ImGui::Begin("GlobalConstant Editor", &activation);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역
			DrawLayout();

			ImGui::End();
		}

		void Module_GlobalConstant::Finalize()
		{

		}

		void Module_GlobalConstant::DrawLayout()
		{
			// Spacing, 바꾸지 않는다는 전제로 static으로 구현
			static auto spacing = ImGui::GetStyle().ItemSpacing;

			auto region = ImGui::GetContentRegionAvail();

			DrawList(region);
		}

		void Module_GlobalConstant::DrawList(const ImVec2& region)
		{
			ImGui::BeginChild("GlobalConstant", region, true);
			static int idx = 0;
			imgui::BeginSection_2Col(idx, "GlobalConstant List", ImGui::GetContentRegionAvail().x, 0.2f);
			imgui::data::DrawDataField(gc.pod, true);
			imgui::EndSection();
			ImGui::EndChild();
		}
	}
}

