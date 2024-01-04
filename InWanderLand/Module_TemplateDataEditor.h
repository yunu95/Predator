/// 2023. 12. 27 김상준
/// TemplateData 편집을 위한 Editor Module

#pragma once

#include "Singleton.h"
#include "EditorModule.h"

#include "imgui.h"

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
		public:
			Module_TemplateDataEditor();
			virtual ~Module_TemplateDataEditor();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		private:
			// TemplateData 리스트와 선택한 리스트에 대한 내용을 출력하는 두 패널을 그려주는 함수
			void DrawLayout();
			// TemplateData 리스트를 그리는 함수
			void DrawList(const ImVec2& region);
			// 선택한 TemplateData 의 내용을 출력하는 패널을 그리는 함수
			void DrawTemplateDataPanel(const ImVec2& region);

		private:
			TemplateDataManager& tdManager;
			ITemplateData* selectedData;
		};
	}
}
