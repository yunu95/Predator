#include "InWanderLand.h"
#include "Module_TemplateDataEditor.h"
#include "Application.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		Module_TemplateDataEditor::Module_TemplateDataEditor()
			: tdManager(TemplateDataManager::GetSingletonInstance()), selectedData(nullptr)
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
			DrawLayout();

			ImGui::End();
		}

		void Module_TemplateDataEditor::Finalize()
		{

		}

		void Module_TemplateDataEditor::DrawLayout()
		{
			// Spacing, 바꾸지 않는다는 전제로 static으로 구현
			static auto spacing = ImGui::GetStyle().ItemSpacing;

			auto region = ImGui::GetContentRegionAvail();

			DrawList(ImVec2(region.x * 0.2 - spacing.x/2, region.y - spacing.y/2));
			ImGui::SameLine();
			DrawTemplateDataPanel(ImVec2(region.x * 0.8 - spacing.x/2, region.y - spacing.y/2));
		}

		void Module_TemplateDataEditor::DrawList(const ImVec2& region)
		{
			ImGui::BeginChild("TemplateDataList", region, true);
			//if (ImGui::CollapsingHeader("Terrain"))
			//{

			//}

			if (ImGui::CollapsingHeader("Units"))
			{
				for (auto each : tdManager.GetDataList(DataType::UnitData))
				{
					if (ImGui::Selectable(static_cast<Unit_TemplateData*>(each)->GetDataKey().c_str(), selectedData == each))
					{
						if (selectedData == each)
						{
							selectedData = nullptr;
						}
						else
						{
							selectedData = each;
						}
					}
				}
			}
			else
			{
				if (selectedData != nullptr && tdManager.GetDataType(selectedData) == DataType::UnitData)
				{
					selectedData = nullptr;
				}
			}

			if (ImGui::CollapsingHeader("Ornaments"))
			{
				for (auto each : tdManager.GetDataList(DataType::OrnamentData))
				{
					if (ImGui::Selectable(static_cast<Ornament_TemplateData*>(each)->GetDataKey().c_str(), selectedData == each))
					{
						if (selectedData == each)
						{
							selectedData = nullptr;
						}
						else
						{
							selectedData = each;
						}
					}
				}
			}
			else
			{
				if (selectedData != nullptr && tdManager.GetDataType(selectedData) == DataType::OrnamentData)
				{
					selectedData = nullptr;
				}
			}

			ImGui::EndChild();
		}

		void Module_TemplateDataEditor::DrawTemplateDataPanel(const ImVec2& region)
		{
			ImGui::BeginChild("SelectedTemplateData", region, true);
			if (selectedData != nullptr)
			{
				switch (tdManager.GetDataType(selectedData->GetDataKey()))
				{
					case DataType::UnitData:
					{
						DrawTemplateDataPOD<Unit_TemplateData>(selectedData);
						break;
					}
					case DataType::OrnamentData:
					{
						DrawTemplateDataPOD<Ornament_TemplateData>(selectedData);
						break;
					}
					default:
						break;
				}
			}
			ImGui::EndChild();
		}
	}
}
