/// 2024. 06. 24 김상준
/// Animation 관련 편집을 위한 Editor Module

#pragma once

#include "Singleton.h"
#include "EditorModule.h"

#include "imgui.h"
#include "TemplateDataList.h"
#include "imgui_ModuleData.h"

namespace application
{
	class GlobalConstant;

	namespace editor
	{
		class TemplateDataManager;
		class Unit_TemplateData;
	}
}

namespace application
{
	namespace editor
	{
		class Module_AnimationEditor
			: public EditorModule, public Singleton<Module_AnimationEditor>
		{
			friend class Singleton<Module_AnimationEditor>;

		public:
			virtual ~Module_AnimationEditor();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		private:
			Module_AnimationEditor();

			// 한 패널에 레이아웃을 작성하는 함수
			void DrawLayout();
			// Data 리스트를 그리는 함수
			void DrawList(Unit_TemplateData* unitTemplate);

			std::string selectedTemplateUnitKey = "None";
			int startAnim = 0;
			int destAnim = 0;
		};
	}
}
