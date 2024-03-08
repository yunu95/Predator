/// 2023. 12. 27 김상준
/// TemplateData 편집을 위한 Editor Module

#pragma once

#include "Singleton.h"
#include "EditorModule.h"

#include "imgui.h"
#include "TemplateDataList.h"
#include "imgui_ModuleData.h"

namespace application
{
	namespace editor
	{
		class TemplateDataManager;
		class ITemplateData;
	}
}

namespace application
{
	namespace editor
	{
		class Module_TemplateDataEditor
			: public EditorModule, public Singleton<Module_TemplateDataEditor>
		{
			friend class Singleton<Module_TemplateDataEditor>;

		public:
			virtual ~Module_TemplateDataEditor();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

			ITemplateData* GetSelectedTemplateData();

		private:
			Module_TemplateDataEditor();

			// TemplateData 리스트와 선택한 리스트에 대한 내용을 출력하는 두 패널을 그려주는 함수
			void DrawLayout();
			// TemplateData 리스트를 그리는 함수
			void DrawList(const ImVec2& region);
			// 선택한 TemplateData 의 내용을 출력하는 패널을 그리는 함수
			void DrawTemplateDataPanel(const ImVec2& region);
			// ITemplateData 를 해당 TemplateData 로 환원하여 해당 데이터의 pod 내용을 그려내는 함수
			template <typename T> requires std::is_base_of_v<ITemplateData, T>
			void DrawTemplateDataPOD(const ITemplateData* data)
			{
				static int idx = 0;
				imgui::BeginSection_2Col(idx, "Field List", ImGui::GetContentRegionAvail().x, 0.2f);
				// 해당 함수를 호출할 때, 이미 dynamic cast 검증이 되었다고 가정함
				auto td = static_cast<T*>(const_cast<ITemplateData*>(data));
				imgui::data::DrawDataField(td->pod);
				imgui::EndSection();
			}

		private:
			TemplateDataManager& tdManager;
		};
	}
}
