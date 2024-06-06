/// 2024. 06. 05 김상준
/// Skill 편집을 위한 Editor Module
/// GC 를 기반하여 작업할 수 있도록 지원함

#pragma once

#include "Singleton.h"
#include "EditorModule.h"

#include "PodStructs.h"
#include "SkillType.h"

#include <memory>

namespace application
{
	namespace editor
	{
		class Module_Skill
			: public EditorModule, public Singleton<Module_Skill>
		{
			friend class Singleton<Module_Skill>;

		public:
			virtual ~Module_Skill();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		private:
			Module_Skill();

			// 한 패널에 레이아웃을 작성하는 함수
			void DrawLayout();
			// Data 리스트를 그리는 함수
			void DrawList(const ImVec2& region);

		private:
			POD_Enum<SkillType::Enum> selectedSkill;
		};
	}
}
