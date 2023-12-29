/// 2023. 12. 27 김상준
/// TemplateData 편집을 위한 Editor Module

#pragma once

#include "Singleton.h"
#include "EditorModule.h"

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

		};
	}
}
