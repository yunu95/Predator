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
	class GlobalConstant;

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
		class Module_GlobalConstant
			: public EditorModule, public Singleton<Module_GlobalConstant>
		{
			friend class Singleton<Module_GlobalConstant>;

		public:
			virtual ~Module_GlobalConstant();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		private:
			Module_GlobalConstant();

			// 한 패널에 레이아웃을 작성하는 함수
			void DrawLayout();
			// Data 리스트를 그리는 함수
			void DrawList(const ImVec2& region);

		private:
			GlobalConstant& gc;
		};
	}
}
